#include "Sockets.hpp"

Sockets::Sockets(Parser& parser) : _parser(parser), _sess_id(1234) {
	std::srand(std::time(NULL));
}

Sockets::~Sockets() {}

void	Sockets::accept(int sock_fd) {
	struct	sockaddr	address;
	socklen_t	si = sizeof(address);
	int new_s_fd = ::accept(sock_fd, &address, &si);
	if (new_s_fd < 0)	return ;
	//
	Server		*target = this->_fd_to_server.find(sock_fd)->second;
	this->_fd_to_server[ new_s_fd ] = target;
	this->_kqueue.SET_QUEUE(new_s_fd, EVFILT_READ, 1);
	std::cout << KGRN << "Accept new connection: " << KNRM << KCYN << new_s_fd << KNRM << std::endl;
}

void	Sockets::recvFrom(int sock_fd) {
	std::cout << "receiving from: " << KCYN << sock_fd << KNRM << std::endl;
	Server	*serv = this->_fd_to_server.find(sock_fd)->second;
	std::map<int, std::pair<Request, Response> >::iterator	pai = serv->_requests.find(sock_fd);
	if (pai == serv->_requests.end()) {
		std::pair<Request, Response>	new_pair;
		serv->_requests[ sock_fd ] = new_pair;
		serv->_requests[ sock_fd].first.set_locations(serv->_locations);
		this->recvFrom(sock_fd);
		return ;
	}
	pai->second.first.recvRequest(sock_fd);
	if (pai->second.first.getState() == DONE || pai->second.first.getState() == ERROR) {
		this->_kqueue.SET_QUEUE(sock_fd, EVFILT_READ, 0);
		this->_kqueue.SET_QUEUE(sock_fd, EVFILT_WRITE, 1);
		pai->second.second._initiate_response(&pai->second.first, *this, serv);
	}
}

void	Sockets::sendTo(int sock_fd) {
	Server	*serv = this->_fd_to_server.find(sock_fd)->second;
	std::map<int, std::pair<Request, Response> >::iterator	pai = serv->_requests.find(sock_fd);
	pai->second.second.sendResponse(sock_fd, serv);
	if (pai->second.second.get_status() == DONE) {
		if (pai->second.second._connection_type == "keep-alive") {
			this->_kqueue.SET_QUEUE(sock_fd, EVFILT_WRITE, 0);
			this->_kqueue.SET_QUEUE(sock_fd, EVFILT_READ, 1);
			this->resetConn(sock_fd);
		}
		else	this->closeConn(sock_fd);
	}
}

void	Sockets::closeConn(int sock_fd) {
	std::cout << KRED << "closing connection:" << KNRM << KCYN <<  sock_fd << KNRM << std::endl;
	this->resetConn(sock_fd);
	this->_fd_to_server.erase(sock_fd);
	//this->_Cookies.erase();
	this->_kqueue.SET_QUEUE(sock_fd, 0, 0);
	close(sock_fd);
}

void	Sockets::resetConn(int sock_fd) {
	std::cout << KWHT << "reseting request/response cycle: " << KNRM << KCYN << sock_fd << KNRM << std::endl;
	this->_fd_to_server[ sock_fd ]->closeConn(sock_fd);
}

void	Sockets::cleanUp() {} // TODO

void	Sockets::incr_sess_id() {
	size_t	r_num = std::rand() % 1000;
	if (this->_sess_id + r_num >= ULONG_MAX) this->_sess_id = 0;
	this->_sess_id += r_num;
}
size_t	Sockets::get_sess_id() { return this->_sess_id; }

///////

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

int	__calc_new_range(int old_value, int old_min, int old_max, int new_min, int new_max) {
	if (old_max==old_min||new_min==new_max) return new_min;
	if (old_value == old_min) return new_min;
	/*else if (old_value>old_max) return new_max;*/
	return ( ((old_value-old_min)*(new_max-new_min))/(old_max-old_min)+new_min);
}

static	std::string	s_encryptor(std::string input, std::string key, int mode) {
	int		i=0;
	while (key.size() < input.size()) { if (i >= key.size()) i = 0; key.append(&key[i]); }
	for (i=0; i < input.size();i++) input[i]+=mode*13;
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
	std::string estab_session_id = response._request->get_headers().cookie;
	std::string user_name = this->form_user_name(*response._request);
	std::cout << "client user agent:" << user_name << std::endl;
	std::string session_id = this->get_cookie(user_name, "session");
	//
	if (!estab_session_id.empty()) {
		std::string cur_session_id = get_cookie_value(estab_session_id, "session=");
		if (s_encryptor(cur_session_id, user_name, -1) == session_id) {
			std::cout << KCYN"already established/valid session: [" << cur_session_id << "] <- "<<session_id<<KNRM<<std::endl;
			response.set_session_id(s_encryptor(cur_session_id, user_name, 1));
			return ;
		}
		else if (!session_id.empty()) {
			std::cout << KCYN"remembering client of their id: [" << session_id << "]" << KNRM<<std::endl;
			response._new_session = true;
			response.set_session_id(s_encryptor(session_id, user_name, 1));
		}
	}
	if (session_id.empty()) {
		this->incr_sess_id();
		session_id = std::to_string(this->get_sess_id());
		this->set_Cookies(user_name, "session=" + session_id + "; ");
		std::string encr_id = s_encryptor(session_id, user_name, 1);
		response.set_session_id(encr_id);
		response._new_session = true;
		std::cout << KCYN"registering a new session: [" << encr_id << "] <- " << session_id <<KNRM<<std::endl;
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
	std::memset(buffer, 0, sizeof(buffer));

	int	pos = cookie.find("; ");
	while (input.read(buffer, pos))
	{
		std::string buff(buffer);	// something
		int eq_pos = buff.find("=");
		if (eq_pos != buff.npos) {
			f_half = buff.substr(0, eq_pos);
			s_half = buff.substr(eq_pos + 1);
			if (!f_half.empty()&&!s_half.empty()
				&&f_half.find_first_not_of(' ')!=f_half.npos
				&&s_half.find_first_not_of(' ')!=s_half.npos)
				new_cookies[ f_half ] = s_half;
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
	return i->second;
}

std::string	Sockets::get_client(std::string name, std::string value) {
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_Cookies.begin();
	for (; it != this->_Cookies.end(); ++it) {
		std::map<std::string, std::string>::iterator i = it->second.find(name);
		if (i != it->second.end() && i->second == value)	return it->first;
	}
	return	"";
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
				else if (events[i].filter == EVFILT_READ)
				{
					it = this->_fd_to_server.find(events[i].ident);
					if (it != this->_fd_to_server.end() && it->first == it->second->_socket)
						this->accept(events[i].ident);
					else	this->recvFrom(events[i].ident);
				}
				else if (events[i].filter == EVFILT_WRITE) {
					this->sendTo(events[i].ident);
				}
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
	std::vector<Server*>			servers = this->_parser.getServers();

	for (sit it = servers.begin(); it != servers.end(); it++) {
		initAddInfo(hints);
		if (getAddrInfo(&hints, &res, it) < 0)
			continue;
		if ((sock = createSocket(res, it)) < 0)
			continue;
		freeaddrinfo(res);
		if (listen(sock, 100) < 0)
			std::cerr << YELLOW << "server: " << (*it)->getHost() << ":" << (*it)->getPort() << " is enable to start." << RESET << std::endl;
		(*it)->_socket = sock;
		this->_kqueue.SET_QUEUE(sock, EVFILT_READ, 1);
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
