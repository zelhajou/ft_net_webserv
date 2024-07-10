#include "Sockets.hpp"

Sockets::Sockets(Parser& parser) : _parser(parser) {}

Sockets::~Sockets() {}

void	Sockets::accept() {} // TODO

void	Sockets::recvFrom() {} // TODO

void	Sockets::sendTo() {} // TODO

void	Sockets::closeConn(int fd) {
	close(fd);
	this->_fd_to_server[fd]->closeConn(fd);
	this->_fd_to_server.erase(fd);
}

void	Sockets::cleanUp() {} // TODO

void	Sockets::kqueueLoop() {} // TODO

static void initAddInfo(struct addrinfo& ai) {
	memset(&ai, 0, sizeof(struct addrinfo));
	ai.ai_family = AF_UNSPEC;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_flags = AI_PASSIVE;
}

static int getAddrInfo(struct addrinfo *hints, struct addrinfo **res, sit it) {
	int					rval;

	if ((rval = getaddrinfo((*it)->getHost().c_str(), (*it)->getPort().c_str(), hints, res)) != 0) {
		std::cerr << YELLOW << "server: " << (*it)->getHost() << ":" << (*it)->getPort() << " is enable to start." << RESET << std::endl;
		return (-1);
	}
	return (0);
}

static int createSocket(struct addrinfo *res, sit it) {
	struct addrinfo		*tmp;
	int					sock;
	int					yes = 1;

	tmp = res;
	while (tmp) {
		sock = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (sock > 0) {
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) >= 0) {
				 if (bind(sock, tmp->ai_addr, tmp->ai_addrlen) >= 0) {
					return (sock);
       			 }
			}
		}
		tmp = tmp->ai_next;
	}
	std::cerr << YELLOW << "server: " << (*it)->getHost() << ":" << (*it)->getPort() << " is enable to start." << RESET << std::endl;
	return (-1);
}

void	Sockets::startServers() {
	struct addrinfo		hints;
	struct addrinfo		*res;
	int					sock;
	int					nbr = 0;

	for (sit it = this->_parser.getServers().begin(); it != this->_parser.getServers().end(); it++) {
		initAddInfo(hints);
		if (getAddrInfo(&hints, &res, it) < 0)
			continue;
		if ((sock = createSocket(res, it)) < 0)
			continue;
		freeaddrinfo(res);
		if (listen(sock, 100) < 0)
			std::cerr << YELLOW << "server: " << (*it)->getHost() << ":" << (*it)->getPort() << " is enable to start." << RESET << std::endl;
		this->_kqueue.SET_QUEUE(sock, EVFILT_READ, EV_ADD);
		this->_fd_to_server[sock] = *it;
		nbr++;
	}
	if (nbr == 0)
		throw std::runtime_error(RED + std::string("No server started") + RESET);
}

void	Sockets::run() {
	this->startServers();
	this->kqueueLoop();
}
