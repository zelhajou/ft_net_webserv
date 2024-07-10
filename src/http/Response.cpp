#include "Response.hpp"

Response::Response(): status(FIRST_LINE), _fd(-1), _has_body(false) {
	this->_sent[0] = 0;
	this->_sent[1] = 0;
}
Response::~Response() {}

size_t	Response::get_file_size() {
	std::streampos	fpos = this->_file.tellg();
	this->_file.seekg(0, std::ios::end);
	return (this->_file.tellg()-fpos);
}

void	Response::_initiate_response(Request &req) {
	this->_request = req;
	if (req.get_status() == OK && req._first_line.method == "GET")
	{
		this->_has_body = true;
		this->_file.open( req._first_line, std::ios::in | std::ios::binary );
		if (!this->_file)
		{
			this->_request._status = 500;
			this->_has_body = false;
		}
		else	this->_file_size = this->get_file_size();
	}
}

e_parser_status	Response::get_status() { return this->status; }

size_t	Response::form_headers(Server *server) {
	// form them
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
		else	this->_sent[0] += sent_res;

		if (this->_sent[0] >= this->_sent[1]) {
			if (this->_has_body) {
				this->_sent[0] = 0;
				this->_sent[1] = 0;
				this->status = BODY;
			}
			else	this->status = DONE;
		}
		else	this->headers = this->headers.substr(sent_res);
	}
	else if (this->status == BODY)
	{
		if (!this->_sent[1])
			this->_sent[1] = this->_file_size;
		this->_sent[0] += send();
	}
}

void	Resopnse::GET() {}

void	Response::DELETE() {}
