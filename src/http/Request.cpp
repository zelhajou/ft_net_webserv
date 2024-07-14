#include "Request.hpp"

Request::Request(/*std::map<std::string, Location>& locations*/) : _fd(-1), _state(FIRST_LINE), _recv(0), _status(OK), _has_body(false), _timeout(0) {
	/*for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); it++) {
		LocationNode*	node = new LocationNode;
		node->name = it->first;
		node->location = &it->second;
		::insert(this->_location_tree, node, ::cmp);
	}
	setlvl(this->_location_tree);*/
}

void	Request::set_locations(std::map<std::string, Location>& locations) {
	for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); it++) {
		LocationNode*	node = new LocationNode;
		node->name = it->first;
		node->location = &it->second;
		::insert(this->_location_tree, node, ::cmp);
	}
	setlvl(this->_location_tree);
}

Request::Request(const Request &R) { *this = R; }
Request	&Request::operator = (const Request &R) { return *this; }

Request::~Request() {}

void	Request::parse_uri() {

	size_t		pos;
	int			depth = 0;
	std::string	uri = this->_first_line.uri;
	std::string	uri_tmp;
	std::string simple_uri = "/";

	while ((pos = uri.find("/")) != std::string::npos) {
		uri_tmp = uri.substr(0, pos);
		if (uri_tmp == "..") {
			simple_uri = simple_uri.substr(0, simple_uri.find_last_of("/"));
			depth--;
		}
		else if (uri_tmp != "." && uri_tmp != "")
			depth++;
		if (depth < 0)
			{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
		uri = uri.substr(pos + 1);
	}
	if (pos > 0 && uri != ".." && uri != "." && uri != "")
		simple_uri += uri;
	this->_first_line.uri = simple_uri;
}

void	Request::check_uri() {
	if (this->_state != FIRST_LINE) return ;

	struct stat		st;
	//std::cout << "before parse_uri:" << this->_first_line.uri << std::endl;
	parse_uri();
	//std::cout << "after parse_uri:" << this->_first_line.uri << std::endl;
	if (this->_state == ERROR) return ;
	Location*		loc = ::search(this->_location_tree, this->_first_line.uri, ::cmp);
	if (loc)
		this->_first_line.uri = loc->getRoot() + this->_first_line.uri;
	/*if (loc == NULL)
		{
			std::cout << "didnt 1 found(" << this->_first_line.uri << std::endl;
			(this->_status = NOT_FOUND, this->_state = ERROR); return ;
		}
	if (loc->getReturnCode() != 0)
		{(this->_status = (e_status)loc->getReturnCode(), this->_state = DONE); return ;}
	if (loc->getMethod().find(this->_first_line.method) == std::string::npos)
		{(this->_status = NOT_IMPLEMENTED, this->_state = ERROR); return ;}
	this->_first_line.uri.replace(0, loc->getLocation().size(), loc->getRoot());
	if (stat(this->_first_line.uri.c_str(), &st) == -1)
		{
			std::cout << "didnt 2 found(" << this->_first_line.uri << std::endl;
			(this->_status = NOT_FOUND, this->_state = ERROR); return ;
		}
	if (!S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode))
		{(this->_status = FORBIDDEN, this->_state = ERROR); return ;}
	if (S_ISDIR(st.st_mode) && loc->getAutoindex() == false && loc->getIndex().empty()) // TODO: check if non of the index files are present
		{(this->_status = FORBIDDEN, this->_state = ERROR); return ;}
	if (loc->getIndex().size() > 0) { // TODO: check if there is a valid index
		this->_first_line.uri += loc->getIndex();
		if (stat(this->_first_line.uri.c_str(), &st) == -1)
			{
				std::cout << "didnt found(" << this->_first_line.uri << std::endl;
				(this->_status = NOT_FOUND, this->_state = ERROR); return ;
			}
		if (!S_ISREG(st.st_mode))
			{(this->_status = FORBIDDEN, this->_state = ERROR); return ;}
		
	}*/
}

void Request::parse_first_line() {
	if (this->_state != FIRST_LINE) return ;

	size_t			pos;

	//std::cout << "body from first_line parser:" << this->_body << std::endl;
	if ((pos = this->_body.find("\r\n")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return;}

	//	method
	if ((pos = this->_body.find(" ")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_first_line.method = this->_body.substr(0, pos);
	//std::cout << "got method:" << this->_first_line.method << std::endl;
	this->_body = this->_body.substr(pos + 1);

	//	uri
	if ((pos = this->_body.find(" ")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_first_line.uri = this->_body.substr(0, pos);
	//std::cout << "got uri:" << this->_first_line.uri << std::endl;
	this->_body = this->_body.substr(pos + 1);

	//	version
	if ((pos = this->_body.find("\r\n")) == std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	this->_first_line.version = this->_body.substr(0, pos);
	//std::cout << "got version:" << this->_first_line.version << std::endl;
	this->_body = this->_body.substr(pos + 1);


	if (this->_first_line.method != "GET" && this->_first_line.method != "POST" && this->_first_line.method != "DELETE")
		{(this->_status = NOT_IMPLEMENTED, this->_state = ERROR); return ;}
	if (this->_first_line.version != "HTTP/1.1")
		{(this->_status = HTTP_VERSION_NOT_SUPPORTED, this->_state = ERROR); return ;}
	if (this->_first_line.uri.find("..") != std::string::npos)
		{(this->_status = BAD_REQUEST, this->_state = ERROR); return ;}
	//
	//	first line new line
	this->_body = this->_body.substr(1);
	//
	check_uri();
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
		//std::cout << "LINE:" << line << ";\n";
		if (line.empty())
			{this->_state = BODY; break;}
		this->_body = this->_body.substr(pos + 2);
		if ((pos = line.find(":")) == std::string::npos) {
			this->_status = BAD_REQUEST;
			this->_state = ERROR;
			return ;
		}
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
		else if (key == "Cookie")
			this->_headers.cookie = value;
		else if (key == "User-Agent")
			this->_headers.user_agent = value;
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
	while (this->_body.size() 
		&& (this->_body[0] == '\r' || this->_body[0] == '\n'))
		this->_body = this->_body.substr(1);
	
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

void	Request::recvRequest(int sock_fd) {
	char		buffer[BUFFER_SIZE];
	int			ret;

	if (this->_state == DONE || this->_state == ERROR) return ;
	if ((ret = recv(sock_fd, buffer, BUFFER_SIZE, 0)) == -1) {
		std::cout << "DONE not peacefully\n";
		this->_status = INTERNAL_SERVER_ERROR;
		this->_state = ERROR;
		return ;
	}
	if (ret == 0)
	{
		std::cout << "DONE peacefully\n";
		(this->_state = DONE); return ;
	}
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
void		Request::setStatus(e_status new_status) { this->_status = new_status; }
