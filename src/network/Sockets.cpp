#include "Sockets.hpp"

Sockets::Sockets(Parser& parser) : _parser(parser) {}

Sockets::~Sockets() {}

void	Sockets::accept(int sock_fd) {
	struct	sockaddr	address;
	socklen_t	si = sizeof(address);
	int new_s_fd = accept(sock_fd, &address, &si);
	if (new_s_fd < 0)	return ;
	//
	Server		*target = this->_fd_to_server.find(sock_fd)->second;
	//
	this->_fd_to_server[ new_s_fd ] = target;
}

void	Sockets::recvFrom(int sock_fd) {
	Server	*serv = this->_fd_to_server.find(sock_fd)->second;
	std::pair<Request, Response>::iterator	pai = serv->_requests.find(sock_fd);
	if (pai == serv->_requests.end)
	{
		std::pair<Request, Response>	new_pair;
		serv->_requests[ sock_fd ] = new_pair;
		this->recvFrom(sock_fd);
		return ;
	}
	pai->second.first.recvRequest();
	if (pai->second.first.get_parser_status() == DONE || pai->second.first.get_parser_status() == ERROR)
	{
		pai->second.second._initiate_response(pai->second.first);
		this->_kqueue.QUEUE_SET(sock_fd, EVFILT_WRITE, EV_ADD);
	}
}

void	Sockets::sendTo(int sock_fd) {
	Server	*serv = this->_fd_to_server.find(sock_fd)->second;
	std::pair<Request, Response>::iterator	pai = serv->_requests.find(sock_fd);
	pai->second.second.sendResponse(sock_fd, serv);
	if (pai->second.second.get_status() == DONE)
	{
		this->_kqueue.QUEUE_SET(sock_fd, EVFILT_READ, EV_ADD);
		this->resetConn(sock_fd);
	}
	else if (pai->second.second.get_status() == ERROR)	this->closeConn(sock_fd);
}

void	Sockets::closeConn(int sock_fd) {
	this->resetConn(sock_fd);
	this->_fd_to_server.erase(sock_fd);
	close(sock_fd);
}

void	Sockets::resetConn(int sock_fd) {
	this->_fd_to_server[ sock_fd ]->closeConn(sock_fd);
}

void	Sockets::cleanUp() {} // TODO

void	Sockets::kqueueLoop() {
	int	n, target;
	for (;;)
	{
		struct	kevent	events[ this->_kqueue.get_current_events() ];
		n = this->_kqueue.QUEUE_CHECK(events);
		if (n > 0)
			for (int i=0; i < n; i++)
			{
				if (events[i].flags & EV_ERROR || events[i].fflags & EV_ERROR
					|| events[i].flags & EV_EOF || events[i].fflags & EV_EOF)
					this->closeConn(events[i].ident);
				else if (events[i].filter & EVFILT_READ)
				{
					target = this->_fd_to_sever.find(events[i].ident);
					if (target != this->_fd_to_server.end() && target->first == target->second->socket)
						this->accept(events[i].ident);
					else	this->recvFrom(events[i].ident);
				}
				else if (events[i].filter & EVFILT_WRITE)	this->sendTo(events[i].ident);
			}
	}
}

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
