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
	this->_fd_to_server[ new_s_fd ] = target;
	//
	this->_kqueue.SET_QUEUE(new_s_fd, EVFILT_READ, EV_ADD);
	//	setting cookie test
	this->set_cookie(this->form_user_name(target, new_s_fd), "cookie=test; ");
}

void	Sockets::recvFrom(int sock_fd) {
	Server	*serv = this->_fd_to_server.find(sock_fd)->second;
	std::map<int, std::pair<Request, Response> >::iterator	pai = serv->_requests.find(sock_fd);
	if (pai == serv->_requests.end())
	{
		std::pair<Request, Response>	new_pair;
		serv->_requests[ sock_fd ] = new_pair;
		this->recvFrom(sock_fd);
		return ;
	}
	pai->second.first.recvRequest();
	if (pai->second.first.getState() == DONE || pai->second.first.getState() == ERROR)
	{
		pai->second.second._initiate_response(pai->second.first, *this);
		this->_kqueue.SET_QUEUE(sock_fd, EVFILT_WRITE, EV_ADD);
	}
}

void	Sockets::sendTo(int sock_fd) {
	Server	*serv = this->_fd_to_server.find(sock_fd)->second;
	std::map<int, std::pair<Request, Response> >::iterator	pai = serv->_requests.find(sock_fd);
	pai->second.second.sendResponse(sock_fd, serv);
	if (pai->second.second.get_status() == DONE && this->_connection_type == "keep-alive")
	{
		this->_kqueue.QUEUE_SET(sock_fd, EVFILT_READ, EV_ADD);
		this->resetConn(sock_fd);
	}
	else						this->closeConn(sock_fd);
}

void	Sockets::closeConn(int sock_fd) {
	this->resetConn(sock_fd);
	this->_fd_to_server.erase(sock_fd);
	this->_Cookies.erase();
	close(sock_fd);
}

void	Sockets::resetConn(int sock_fd) {
	this->_fd_to_server[ sock_fd ]->closeConn(sock_fd);
}

void	Sockets::cleanUp() {} // TODO

void	Sockets::set_Cookies(std::string client, std::string cookie) {
	std::map<std::string, std::string>	new_cookies;
	std::stringstream			input(cookie);
	std::string			buffer, f_half, s_half;
	//
	while (input.read(buffer, "; "))
	{
		if (!input.gcount() || buffer.empty()) continue ;
		int	spos = buffer.find('=');
		if (spos != buffer.npos)
		{
			f_half = buffer.substr(0, spos);
			s_half = buffer.substr(spos + 1);
			if (f_half.empty() || s_half.empty()
				|| f_half.find_first_not_of(' ') == f_half.npos
				|| s_half.find_first_not_of(' ') == s_half.npos)
				continue ;
			new_cookies[ f_half ] = s_half;
		}
	}
	//
	if (new_cookies.size())	this->_Cookies[ client ] = new_cookies;
}

std::map<std::string, std::map<std::string, std::string> >::iterator	&Sockets::get_Cookies(std::string client) { return this->_Cookies.find( client ); }

std::string	Sockets::get_cookie(std::string client, std::string var) {
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->get_Cookies(client);
	if (it == this->_Cookies.end())	return "";
	std::map<std::string, std::string>::iterator i = it->second.find(var);
	if (i == it->second.end())		return "";
	return i->second;
}

std::string	Sockets::get_client(std::string name, std::string value) {	// not recommended
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_Cookies.begin();
	for (; it != this->_Cookies.end(); ++it) {
		std::map<std::string, std::string>::iterator i = it->second.begin();
		for (; i!=it->second.end(); ++i)
			if (i->first == name && i->second == value)	return it->first;
	}
	return	"";
}

static	std::string	clean_up_stuff(std::string input, std::string garbage, std::string target) {
	if (input.empty()||garbage.empty()||garbage.size() != target.size())	return "";
	int				pos;
	std::string			shgarbage;
	for (int i=0; i < garbage.size(); i++)
	{
		shgarbage = garbage.substr(i, i+1);
		while (true) {
			pos = input.find(shgarbage);
			if (pos == input.npos)	break;
			input.replace(pos, 1, target.substr(i, i+1));
		}
	}
	return	input;
}

std::string	Sockets::form_user_name(Request &request, int sock_fd) {
	std::string	host = clean_up_chars(request._headers.host, ";=", ".:");
	std::string	user_agent = clean_up_chars(request._headers.user_agent, ";=", ".:");
	return	(host+user_agent+std::to_string(sock_fd));
}

void	Sockets::kqueueLoop() {
	std::map<int, Server*>::iterator	it;
	int	n;
	for (;;)
	{
		struct	kevent	events[ this->_kqueue.get_current_events() ];
		n = this->_kqueue.CHECK_QUEUE(events);
		if (n > 0)
			for (int i=0; i < n; i++)
			{
				if (events[i].flags & EV_ERROR || events[i].fflags & EV_ERROR
					|| events[i].flags & EV_EOF || events[i].fflags & EV_EOF)
					this->closeConn(events[i].ident);
				else if (events[i].filter & EVFILT_READ)
				{
					it = this->_fd_to_sever.find(events[i].ident);
					if (it != this->_fd_to_server.end() && it->first == it->second->socket)
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

std::string	Sockets::get_mime_type(std::string ext) { return	this->_mime.getMIME(ext); }
