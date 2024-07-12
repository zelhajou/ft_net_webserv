#include "Sockets.hpp"

Sockets::Sockets(Parser& parser) : _parser(parser), _sessions(0) {}

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
	if (pai->second.second.get_status() == DONE && this->_connection_type == "keep-alive") {
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

void	Sockets::set_current_sessions(int new_count) { this->_sessions = new_count; }
int	Sockets::get_current_sessions() { return this->_sessions; }

///////
static	std::string	xor_encryptor(std::string input, std::string key) {
	int i = 0;
	std::string		output(input.size(), 0);
	while (key.size() < input.size()) {
		if (i >= key.size())	i = 0;
		key.append(&key[i]);
	}
	for (i=0; i < input.size();i++)
		input[i] ^= key[i];
	return	input;
}

static	std::string	get_cookie_value(std::string input, std::string name)
{
	int	pos = input.find(name), epos;
	if (pos == input.npos) return "";
	pos += name.size();
	epos = pos;
	try {
		if (epos > input.size() || input[epos] == input.npos) return "";
		while (input[epos] != ';' && input[epos] != ' ') epos += 1;
		return	input.substr(pos, epos-pos);
	}
	catch (std::exception &l)
	{ return ""; }
}

void		Sockets::check_session(Response &response) {
	std::string estab_session_id = respose._request.get_headers().cookie;
	std::string user_name = this->form_user_name(response._request);
	std::string session_id = this->get_cookie(user_name, "session");
	if (!estab_session_id.empty()) {
		std::string cur_session_id = get_cookie_value(estab_session_id, "session=");
		if (xor_encryptor(cur_session_id, "session") == session_id) {
			response.set_session_id("session=" + cur_session_id + "; ");
			return ;
		}
		else if (!session_id.empty()) {
			response._new_session = true;
			response.set_session_id("session=" + xor_encryptor(session_id) + "; ");
		}
	}
	if (session_id.empty()) {
		this->set_current_sessions(this->get_current_sessions()+1);
		session_id = xor_encryptor(std::to_string(this->get_current_sessions()));
		this->set_Cookies(user_name, "session=" + session_d + "; ");
		response._new_session = true;
	}
}

//////
std::string	Sockets::form_user_name(Request &request) {
	std::string	host = clean_up_stuff(request.get_headers().host, ".;=( ):", "##${_}#");
	std::string	user_agent = clean_up_stuff(request.get_headers().user_agent, ".;=( ):", "##${_}#");
	return	(host+user_agent);
}


void	Sockets::set_Cookies(std::string client, std::string cookie) {
	std::map<std::string, std::string>	new_cookies;
	std::stringstream			input(cookie);
	std::string			f_half, s_half;
	char				buffer[cookie.size()];

	int	pos = cookie.find("; ");
	while (input.read(buffer, pos))
	{
		std::string buff(buffer);
		int eq_pos = buff.find("=");
		if (eq_pos != buff.npos) {
			f_half = buff.substr(0, eq_pos);
			s_half = buff.substr(eq_pos + 1);
			if (!f_half.empty()&&!s_half.empty()
				&&f_half.find_first_not_of(' ')!=f_half.npos
				&&s_half.find_first_not_of(' ')!=s_half.npos)
				new_cookies[ f_half ] = xor_encryptor(s_half, f_half);
		}
		pos = cookie.substr(pos+2).find("; ");
		if (pos == cookie.npos)	break;
		input.get(); input.get();
	}
	if (new_cookies.size())		this->_Cookies[ client ] = new_cookies;
}

std::map<std::string, std::map<std::string, std::string> >::iterator	Sockets::get_Cookies(std::string client) { return this->_Cookies.find( client ); }

std::string	Sockets::get_cookie(std::string client, std::string var) {
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->get_Cookies(client);
	if (it == this->_Cookies.end())	return "";
	std::map<std::string, std::string>::iterator i = it->second.find(var);
	if (i == it->second.end())		return "";
	return xor_encryptor(i->second, var);
}

std::string	Sockets::get_client(std::string name, std::string value) {
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_Cookies.begin();
	for (; it != this->_Cookies.end(); ++it) {
		std::map<std::string, std::string>::iterator i = it->second.find(name);
		if (i != it->second.end() && i->second == xor_encryptor(value, name))	return it->first;
	}
	return	"";
}

static	std::string	clean_up_stuff(std::string input, std::string garbage, std::string target) {
	if (input.empty()||garbage.empty()||garbage.size() != target.size())	return "";
	int				pos;
	for (int i=0; i < garbage.size(); i++)
	{
		std::string	shgarbage(1, garbage[i]);
		while (true) {
			pos = input.find(shgarbage);
			if (pos == input.npos)	break;
			std::string	sec_tar(1, target[i]);
			input.replace(pos, 1, sec_tar);
		}
	}
	return	input;
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
