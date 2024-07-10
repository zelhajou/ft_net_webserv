#include "Response.hpp"

Response::Response(): status(FIRST_LINE), _fd(-1), _has_body(false) {
	this->_sent[0] = 0;
	this->_sent[1] = 0;
}
Response::~Response() {}

void	Response::_initiate_response(Request &req) {
	this->_request = req;
	if (req.get_status() == OK && req._first_line.method == "GET")
		this->_has_body = true;
}

e_parser_status	Response::get_status() { return this->status; }

static	size_t	form_headers(std::string &headers, Request &request, Server *server) {
	// form them
	return headers.size();
}

void	Response::sendResponse(int sock_fd, Server *server) {
	if (this->status == FIRST_LINE)
	{
		this->_sent[1] = form_headers(this->headers, this->_request, server);
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
			if (this->_request._has_body) {
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
		if (!this->_sent[1]) {
			this->
		}
	}
}

void	Resopnse::GET() {}

void	Response::DELETE() {}
