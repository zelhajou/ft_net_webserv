#include "Request.hpp"

Request::Request() : _fd(-1), _recv(0), _state(FIRST_LINE), _status(NONE), _timeout(0), _has_body(false), _hex(true), _chunk_size(0) {
}

Request::Request(const Request &R) { *this = R; }
Request	&Request::operator = (const Request &R) { return *this; }

Request::~Request() {}

void Request::setLocation(std::map<std::string, LocationConfig> &locations) {
	for (std::map<std::string, LocationConfig>::iterator it = locations.begin(); it != locations.end(); it++) {
		LocationNode*	node = new LocationNode;
		node->name = it->first;
		node->location = &it->second;
		::insert(this->_location_tree, node, ::cmp);
	}
	setlvl(this->_location_tree);
}

void	Request::parse_uri() {

	size_t		pos;
	int			depth = 0;
	std::string	uri = this->_first_line.uri;
	std::string	uri_tmp;
	std::string simple_uri = "/";

	while ((pos = uri.find("/")) != std::string::npos) {
		uri_tmp = uri.substr(0, pos);
		if (uri_tmp == "..")
			(simple_uri = simple_uri.substr(0, simple_uri.find_last_of("/")), depth--);
		else if (uri_tmp != "." && uri_tmp != "")
			(simple_uri += uri_tmp + "/", depth++);
		if (depth < 0)
			{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
		uri = uri.substr(pos + 1);
	}
	if (uri == "..")
		(simple_uri = simple_uri.substr(0, simple_uri.find_last_of("/")), depth--);
	else if (uri != "." && uri != "")
		(simple_uri += uri + "/", depth++);
	if (depth < 0)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_first_line.uri = simple_uri;
}

void	Request::handle_cgi() {

}

bool	Request::is_cgi() {
	size_t	pos;

	if ((pos = this->_first_line.uri.find(".php")) != std::string::npos && pos == this->_first_line.uri.size() - 4)
		return true;
	if ((pos = this->_first_line.uri.find(".py")) != std::string::npos && pos == this->_first_line.uri.size() - 3)
		return true;
	return false;
}

void	Request::check_uri() {
	if (this->_state != FIRST_LINE) return ;

	struct stat		st;
	parse_uri();
	if (this->_state == ERROR) return ;
	LocationConfig*		loc = ::search(this->_location_tree, this->_first_line.uri, ::cmp);
	if (loc == NULL)
		{(this->_status = NOT_FOUND, this->_state = ERROR);
		std::cout << "Location Not Found (null)" << std::endl;
		return ;}
	if (loc->return_url.first != NONE)
		{(this->_status = loc->return_url.first, this->_state = DONE);
		std::cout << "Return is set to not NONE" << std::endl;
		return ;}
	if (std::find(loc->allowed_methods.begin(), loc->allowed_methods.end(), this->_first_line.method) == loc->allowed_methods.end())
		{(this->_status = NOT_IMPLEMENTED, this->_state = ERROR);
		std::cout << "Method Not Allowed (not supported inside location)" << std::endl;
		return ;}
	if (is_cgi())
		{handle_cgi(); return ;}
	this->_first_line.uri.replace(0, loc->path.size(), loc->root);
	if (stat(this->_first_line.uri.c_str(), &st) == -1)
		{(this->_status = NOT_FOUND, this->_state = ERROR);
		std::cout << "File Not Found (stat)" << std::endl;
		return ;}
	if (!S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode))
		{(this->_status = FORBIDDEN, this->_state = ERROR);
		std::cout << "Forbidden (not a file or directory)" << std::endl;
		return ;}
	if (loc->index.size() > 0) { // TODO: check if there is a valid index
		this->_first_line.uri += "/" + loc->index;
		if (stat(this->_first_line.uri.c_str(), &st) == -1)
			{(this->_status = NOT_FOUND, this->_state = ERROR);
			std::cout << "File Not Found : " << this->_first_line.uri << std::endl;
			return ;}
		if (!S_ISREG(st.st_mode))
			{(this->_status = FORBIDDEN, this->_state = ERROR);
			std::cout << "Forbidden (not a file)" << std::endl;
			return ;}
		this->_location_type = STATIC;
	}
	else if (S_ISDIR(st.st_mode) && loc->auto_index == true)
		this->_location_type = AUTOINDEX;
	else
		{(this->_status = FORBIDDEN, this->_state = ERROR);
		std::cout << "Forbidden (no index)" << std::endl;
		return ;}
}

void Request::parse_first_line() {
	if (this->_state != FIRST_LINE) return ;

	size_t			pos;

	/* checking the format of the first line */
	if ((pos = this->_body.find("\r\n")) == std::string::npos)
		{(this->_status = URI_TOO_LONG, this->_state = ERROR); return;}
	if ((pos = this->_body.find(" ")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_first_line.method = this->_body.substr(0, pos);
	this->_body = this->_body.substr(pos + 1);
	if ((pos = this->_body.find(" ")) == std::string::npos)
		{(this->_status = URI_TOO_LONG, this->_state = ERROR); return ;}
	this->_first_line.uri = this->_body.substr(0, pos);
	this->_body = this->_body.substr(pos + 1);
	if ((pos = this->_body.find("\r\n")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_first_line.version = this->_body.substr(0, pos);
	this->_body = this->_body.substr(pos + 2);
	/* checking the content of the first line */
	if (this->_first_line.method != "GET" && this->_first_line.method != "POST" && this->_first_line.method != "DELETE")
		{(this->_status = NOT_IMPLEMENTED, this->_state = ERROR);
		std::cout << "Method Not Implemented" << std::endl;
		return ;}
	if (this->_first_line.version != "HTTP/1.1")
		{(this->_status = HTTP_VERSION_NOT_SUPPORTED, this->_state = ERROR);
		std::cout << "HTTP Version Not Supported" << std::endl;
		return ;}
	check_uri();
	if (this->_state == FIRST_LINE)
		this->_state = HEADERS;
}

void Request::parse_headers() {
	if (this->_state != HEADERS) return ;

	size_t			pos;
	std::string		key;
	std::string		value;

	if ((pos = this->_body.find("\r\n\r\n")) == std::string::npos)
		return ;

	while ((pos = this->_body.find("\r\n")) != std::string::npos)
	{
		std::string line = this->_body.substr(0, pos);
		if (line.empty())
			{this->_state = BODY; break;}
		this->_body = this->_body.substr(pos + 2);
		if ((pos = line.find(":")) == std::string::npos)
			{this->_status = BAD_REQUEST; this->_state = ERROR;
			std::cout << "Bad Request (no colon): " << line << std::endl;
			return ;}
		key = line.substr(0, pos);
		value = line.substr(pos);
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
		{this->_status = REQUEST_HEADER_FIELDS_TOO_LARGE; this->_state = ERROR;
		std::cout << "Request Header Fields Too Large" << std::endl;
		return ;}
	if (this->_state == BODY) {
		this->_body = this->_body.substr(2);
		this->_chunked = this->_headers.transfer_encoding == "chunked";
		this->_content_length = std::strtoll(this->_headers.content_length.c_str(), NULL, 10);
	}
}

void Request::handle_chunked() {
	size_t		pos;
	std::string	tmp;

	if ((pos = this->_body.find("\r\n")) == std::string::npos)
		return ;
	tmp = this->_body.substr(0, pos);
	if (_hex) {
		this->_chunk_size = std::strtoll(tmp.c_str(), NULL, 16);
		if (this->_chunk_size == 0)
			{this->_state = DONE; return ;}
		this->_hex = false;
	}
	else {
		if (this->_chunk_size != tmp.size())
			{this->_status = BAD_REQUEST; this->_state = ERROR; return ;}
		this->_hex = true;
		this->_chunked_body.push_back(this->_body.substr(0, pos));
	}
	this->_body = this->_body.substr(pos + 2);
	
}

void Request::parse_body() {
	if (this->_state != BODY) return ;

	if (this->_body.size() == 0)
	{
		this->_state = DONE;
		return ;
	}
	this->_has_body = true;
	if (!this->_chunked && this->_content_length == 0)
		{this->_state = ERROR; this->_status = LENGTH_REQUIRED;
		std::cout << "Length Required: " << this->_body.size() << std::endl;
		std::cout << "BODY: " << this->_body << std::endl;
		return ;}
	if (!this->_chunked && this->_body.size() >= this->_content_length)
		{this->_state = DONE;
		std::cout << "Done" << std::endl;
		return ;}
	if (this->_chunked)
		handle_chunked();
	this->_state = DONE;
}

void	Request::recvRequest() {
	char		buffer[BUFFER_SIZE];
	int			ret;

	if (this->_state == DONE || this->_state == ERROR) return ;
	if ((ret = recv(this->_fd, buffer, BUFFER_SIZE, 0)) == -1) { // old syntax produced a bug
		this->_status = INTERNAL_SERVER_ERROR;
		this->_state = ERROR;
		return ;
	}
	if (ret == 0) {this->_state = DONE;
	std::cout << "Done Recv 0 byte" << std::endl;
	return ;}
	this->_recv += ret;
	this->_body.append(buffer, ret);
	parse_first_line();
	parse_headers();
	parse_body();
}


e_status	Request::getStatus( void ) {
	return this->_status;
}

e_parser_state	Request::getState( void ) {
	return	this->_state;
}

t_first_line	Request::get_first_line() { return this->_first_line; }
t_headers		Request::get_headers() { return this->_headers; }
e_location_type	Request::get_location_type() { return this->_location_type; }
void		Request::set_fd(int sock_fd) { this->_fd = sock_fd; }

void			Request::setStatus(e_status status) { this->_status = status; }
