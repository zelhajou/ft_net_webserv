#include "Sockets.hpp"

static void initAddInfo(struct addrinfo& ai) {
	memset(&ai, 0, sizeof(struct addrinfo));
	ai.ai_family = AF_UNSPEC;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_flags = AI_PASSIVE;
}

static void getAddrInfo(struct addrinfo *hints, struct addrinfo **res, mit it) {
	int					rval;

	if ((rval = getaddrinfo(it->second->getHost().c_str(), it->second->getPort().c_str(), hints, res)) != 0)
		throw std::runtime_error(std::string("Error: getaddinfo: ") + gai_strerror(rval));
}

static int createSocket(struct addrinfo *res) {
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
	throw std::runtime_error(std::string("Error: Server is enable to start."));
	return (-1);
}

void	Sockets::start() {
	struct addrinfo		hints;
	struct addrinfo		*res;
	int					sock;

	for (mit it = this->_fd_to_server.begin(); it != this->_fd_to_server.end(); it++) {
		initAddInfo(hints);
		getAddrInfo(&hints, &res, it);
		sock = createSocket(res);
		freeaddrinfo(res);
		if (listen(sock, 100) < 0)
			throw std::runtime_error(std::string("Error: Server is enable to start."));
		// this->_kqueue.addEvent(sock, EVFILT_READ, EV_ADD, 0, 0, it->second);
	}
}