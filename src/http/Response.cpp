#include "Response.hpp"

Response::Response(): status(FIRST_LINE), _has_body(false) {
	this->_sent[0] = 0;
	this->_sent[1] = 0;
}
Response::~Response() {}

size_t	Response::get_file_size() {
	std::streampos	fpos = this->_file.tellg();
	this->_file.seekg(0, std::ios::end);
	size_t	size = this->_file.tellg()-fpos;
	this->_file.seekg(0, std::ios::beg);
	return (size);
}

void	Response::_initiate_response(Request *req, Sockets &sock) {
	this->_request = req;
	if (req.get_status() == OK && req._first_line.method == "GET")
	{
		this->_has_body = true;
		this->_file.open( req._first_line.uri, std::ios::in | std::ios::binary);
		if (!this->_file) {
			this->_request->getStatus() = 500;
			this->_has_body = false;
		}
		else {
			this->_file_size = this->get_file_size();
			int	ppos = this->_request->get_first_line().uri.rfind(".");
			if (ppos == this->_request->get_first_line().uri.npos) ppos = 0;
			this->_file_type = sock.get_mime_type(this->_request->get_first_line().uri.substr(ppos));
		}
	}
	this->_connection_type = req->get_headers().connection == "close" ? "close" : "keep-alive";
}

e_parser_status	Response::get_status() { return this->status; }

static	std::string	http_code_msg(e_status code)
{
	switch (code) {
		case OK:				return "OK";
		case BAD_REQUEST			return "Bad Request";
		case NOT_FOUND			return "Not Found";
		case INTERNAL_SERVER_ERROR		return "Internal Server Error";
		case NOT_IMPLEMENTED		return "Not Implemented";
		case REDIRECT			return "Redirect";
		case NOT_MODIFIED			return "Not Modified";
		case TOO_MANY_REQUESTS		return "Too Many Requests";
		case REQUEST_ENTITY_TOO_LARGE		return "Request Entity Too Large";
		case REQUEST_HEADER_FIELDS_TOO_LARGE	return "Request Header Fields Too Large";
		case HTTP_VERSION_NOT_SUPPORTED	return "Http Version NOT Supported";
		case URI_TOO_LONG			return "Url Too Long";
		case LENGTH_REQUIRED		return "Length Required";
		case REQUEST_TIMEOUT		return "Request Timeout";
		default				return " ";
	}
}

size_t	Response::form_headers(Server *server) {
	e_status	req_scode = this->_request->getStatus();
	if (this->header.size())	this->header.clear();
	this->header = "HTTP/1.1 " + std::to_string(req_scode) + " " + http_code_msg(req_scode) + CRLF;
	this->header.append("Server: " + server->_server_name + CRLF"Connection: " + this->_connection_type + CRLF);
	if (req_scode == REDIRECT)	this->header.append("Location: "+/**/+CRLF);
	if (req_scode == OK)
	{
		this->header.append("Content-type: " + this->_file_type + CRLF);
		this->header.append("Content-Length: " + std::string(this->_file_size) + CRLF);
	}
	else	this->header.append("Content-Length: 0"CRLF);
	//
	this->header.append(CRLF);
	return this->header.size();
}

void	Response::sendResponse(int sock_fd, Server *server) {
	if (this->status == FIRST_LINE)
	{
		this->_sent[1] = this->form_headers(server);
		this->status = HEADER;
		return ;
	}
	else if (this->status = HEADER)
	{
		int		sent_res;
		sent_res = send(sock_fd, this->headers.c_str(), this->headers.size(), 0);
		if (sent_res < 0)	this->_sent[0] = this->_sent[1];
		else		this->_sent[0] += sent_res;

		if (this->_sent[0] >= this->_sent[1]) {
			if (this->_has_body) {
				this->_sent[1] = 0;
				this->status = BODY;
			}
			else	this->status = DONE;
		}
		else	this->headers = this->headers.substr(sent_res);
	}
	else if (this->status == BODY)
	{
		char	buffer[FILE_READ_BUFFER_SIZE];
		int	read_res;

		std::memset(buffer, 0, sizeof(FILE_RAEAD));
		if (!this->_sent[1]) {
			this->_sent[1] = this->_file_size;
			this->_sent[0] = 0;
		}
		this->_file.read(buffer, FIlE_READ_BUFFER_SIZE);
		read_res = this->_file.gcount();
		for (int bytes_sent=0, temp_perc=0; bytes_sent < read_res;) {
			temp_perc = send(sock_fd, buffer, read_res, 0);
			if (temp_perc < 0) {
				this->_sent[0] = this->_sent[1];
				break ;
			}
			bytes_sent += temp_perc;
		}
		this->_sent[0] += read_res;
		if (this->_sent[0] >= this->_sent[1])	this->status = DONE;
	}
	if (this->status == DONE)	this->_file.close();
}

