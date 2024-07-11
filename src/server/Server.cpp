#include "Server.hpp"

Server::Server() {} // TODO

Server::~Server() {
	for (std::map<int, std::pair<Request, Response> >::iterator i = this->_requests.begin();
		i != this->_requests.end(); ++i)
		this->_requests.erase(i);
}

std::string		Server::getHost() const {
	return (this->_host);
}

std::string		Server::getPort() const {
	return (this->_port);
}

void	Server::closeConn(int fd) {
	std::map<int, std::pair<Request, Response> >::iterator c_fd = this->_requests.find(fd);
	if (c_fd != this->_requests.end())	this->_requests.erase(c_fd);
}
