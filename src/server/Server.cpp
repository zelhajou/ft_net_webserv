#include "Server.hpp"

Server::Server() {} // TODO

Server::~Server() {} // TODO

std::string		Server::getHost() const {
	return (this->_host);
}

std::string		Server::getPort() const {
	return (this->_port);
}
void	Server::closeConn(int fd) {
	close(fd);
	this->_fd_to_server[fd]->closeConn(fd);
	this->_fd_to_server.erase(fd);
}