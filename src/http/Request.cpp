#include "Request.hpp"

Request::Request() : _fd(-1), _recv(0), _state(FIRST_LINE), _status(OK), _timeout(0), _has_body(false) {}

Request::~Request() {}

void Request::parse_first_line() {
	if (this->_state != FIRST_LINE) return ;

	size_t			pos;

	if ((pos = this->_body.find("\r\n")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return;}
	if ((pos = this->_body.find(" ")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_first_line.method = this->_body.substr(0, pos);
	this->_body = this->_body.substr(pos + 1);
	if ((pos = this->_body.find(" ")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_first_line.uri = this->_body.substr(0, pos);
	this->_body = this->_body.substr(pos + 1);
	if ((pos = this->_body.find("\r\n")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_first_line.version = this->_body.substr(0, pos);
	if (this->_first_line.method != "GET" && this->_first_line.method != "POST" && this->_first_line.method != "DELETE")
		{(this->_status = NOT_IMPLEMENTED, this->_state = ERROR); return ;}
	if (this->_first_line.version != "HTTP/1.1")
		{(this->_status = HTTP_VERSION_NOT_SUPPORTED, this->_state = ERROR); return ;}
	if (this->_first_line.uri.find("..") != std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_state = HEADERS;
}

void Request::parse_headers() {
	if (this->_state != HEADERS) return ;

	size_t			pos;
	std::string		key;
	std::string		value;

	while ((pos = this->_body.find("\r\n")) != std::string::npos)
	{
		std::string line = this->_body.substr(0, pos);
		if (line.empty())
			{this->_state = BODY; break;}
		this->_body = this->_body.substr(pos + 2);
		if ((pos = line.find(" ")) == std::string::npos)
			{this->_status = BAD_REQUEST; this->_state = ERROR; return ;}
		key = line.substr(0, pos);
		value = line.substr(pos + 1);
		if (key == "Host")
			this->_headers.host = value;
		else if (key == "Connection")
			this->_headers.connection = value;
		else if (key == "Content-Type")
			this->_headers.content_type = value;
		else if (key == "Content-Length")
			this->_headers.content_length = value;
		else if (key == "Transfer-Encoding")
			this->_headers.transfer_encoding = value;
		else if (key == "Accept")
			this->_headers.accept = value;
	}
	if (this->_state == HEADERS && (this->_body.size() >= BUFFER_SIZE / 2 || this->_recv >= BUFFER_SIZE * 4))
		{this->_status = REQUEST_HEADER_FIELDS_TOO_LARGE; this->_state = ERROR;}
	if (this->_state == BODY) {
		this->_chunked = this->_headers.transfer_encoding == "chunked";
		this->_content_length = std::strtoll(this->_headers.content_length.c_str(), NULL, 10);
	}
}

void Request::parse_body() {
	if (this->_state != BODY) return ;

	if (this->_has_body || this->_body.size() > 0)
		this->_has_body = true;
	if (this->_has_body && !this->_chunked && this->_content_length == 0)
		{this->_state = ERROR; this->_status = LENGTH_REQUIRED; return ;}
	if (this->_has_body && !this->_chunked && this->_body.size() >= this->_content_length)
		{this->_state = DONE; return ;}
	if (this->_has_body && this->_chunked) {
		size_t	pos;
		if ((pos = this->_body.find("\r\n")) == std::string::npos)
			return ;
		size_t	chunk_size = std::strtoll(this->_body.substr(0, pos).c_str(), NULL, 16);
		if (chunk_size == 0)
			{this->_state = DONE; return ;}
		if (this->_body.size() >= pos + 2 + chunk_size + 2)
			this->_body = this->_body.substr(pos + 2 + chunk_size + 2);
	}
	this->_state = DONE;
}

void	Request::recvRequest() {
	char		buffer[BUFFER_SIZE];
	int			ret;

	if (this->_state == DONE || this->_state == ERROR) return ;
	if ((ret = recv(this->_fd, buffer, BUFFER_SIZE, 0)) == -1)
		(this->_status = INTERNAL_SERVER_ERROR, this->_state = ERROR); return ;
	if (ret == 0)
		(this->_state = DONE); return ;
	this->_recv += ret;
	this->_body.append(buffer, ret);

}