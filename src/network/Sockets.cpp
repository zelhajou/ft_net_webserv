#include "Sockets.hpp"


Sockets::Sockets(const Sockets &S) {*this = S;}
Sockets	&Sockets::operator = (const Sockets &S) { (void)S; return *this;}

void	fix_up_signals(void (*f)(int)) {
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, f);
	signal(SIGINT, f);
	signal(SIGKILL, f);
}

static	void	child_ex(int sig_num) {
	std::cout << KCYN"master_process:" << KNRM << " received signal(" << KGRN << sig_num
		<< KNRM << ") exiting..\n";
	exit(sig_num);
}

static	std::string	exec_job(char *executer, char *script, char **env, std::string file_name) {
	std::string	output;
	char		buffer[CGI_PIPE_MAX_SIZE];
	int		pi[2], s;
	pid_t		grandchild;
	if (pipe(pi) < 0)	return "webserv_cgi_status=500; CGI pipe()/open() failure";
	if ((grandchild = fork()) < 0)
		return "webserv_cgi_status=500; CGI fork() failure";
	else if (!grandchild) {
		char	*argv[] = {executer, script, 0};
		if (file_name.size()) {
			int fd = open(file_name.c_str(), R_OK);
			if (fd < 0)	exit(EXIT_FAILURE);
			dup2(fd, 0);
			close(fd);
		}
		dup2(pi[1], 1); close(pi[1]);
		execve(argv[0], argv, env);
		std::cout << KRED"EXECVE_FAILURE:" << strerror(errno) << KNRM << "\n";
		exit(EXIT_FAILURE);
	}
	close(pi[1]);
	output.append("webserv_cgi_status=200;");
	while (true) {
		std::memset(buffer, 0, sizeof(buffer));
		s = read(pi[0], buffer, CGI_PIPE_MAX_SIZE -1);
		if (s <= 0)	break ;
		output.append(buffer, s);
	}
	close(pi[0]);
	std::remove(file_name.c_str());
	return	output;
}

static	int	geta_unix_socket(struct sockaddr_un &address, std::string socket_path) {
	std::memset(&address, 0, sizeof(address));
	int	sock;
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)	return -1;
	address.sun_family = AF_UNIX;
	std::strcpy(address.sun_path, socket_path.c_str());
	return		sock;
}

static	std::string	prepare_launch_and_receive(std::string input, std::string del, std::string semi_del) {
	size_t		pos = input.find(del);
	std::string	s_env = input.substr(0, pos);
	input = input.substr(pos + del.size());
	std::vector<std::string>	v_env;
	for (;(pos = s_env.find(semi_del)) != std::string::npos;) {
		v_env.push_back(s_env.substr(0, pos));
		s_env = s_env.substr(pos + semi_del.size()); }
	pos = 0;
	char	**env = new char*[ v_env.size() + 1 ];
	std::vector<std::string>::iterator	it = v_env.begin();
	for (; it!=v_env.end();++it, ++pos) {
		env[pos] = new char[ it->size() +1 ];
		std::strcpy(env[pos], it->c_str()); }
	env[pos] = 0;
	pos = input.find(del);
	std::string	executer = input.substr(0, pos);
	input = input.substr(pos + del.size());
	pos = input.find(del);
	//////////////////////
	std::string output = exec_job(const_cast<char*>(executer.c_str()),
			const_cast<char*>(input.substr(0, pos).c_str()),
			env,
			input.substr(pos + del.size()));
	//////////////////////
	for (int i=0; env[i]; i++)	delete	env[i];
	delete	[] env;
	return	output;
}

static	void	master_routine(std::string socket_path) {
	fix_up_signals(child_ex);
	std::string	input, output;
	std::string	executer, script;
	struct	sockaddr_un	address;
	int	sock, n, temp_n;
	fd_set	r_set, w_set, r_copy, w_copy;
	FD_ZERO(&r_set); FD_ZERO(&w_set);
	FD_ZERO(&r_copy); FD_ZERO(&w_copy);
	if ((sock = geta_unix_socket(address, socket_path)) < 0)	exit(1);
	FD_SET(sock, &r_set);
	for (int i = 15 ; i ; i--) {
		if (connect(sock, (struct sockaddr*)&address, sizeof(address)) == 0)	break;
		else if (i == 1) { exit(1); } }
	for (;;) {
		r_copy = r_set; w_copy = w_set;
		n = select(sock + 1, &r_copy, &w_copy, NULL, NULL);
		if (!n) continue; else if (n < 0) break;
		if (FD_ISSET(sock, &r_copy)) {
			char	buffer[UNIX_SOCK_BUFFER];
			{	//	<--
				std::memset(buffer, 0, sizeof(buffer));
				temp_n = recv(sock, buffer, UNIX_SOCK_BUFFER - 1, 0/*MSG_DONTWAIT*/);
				if (temp_n <= 0)	break;
				input.append(buffer, temp_n);
			}
			output = prepare_launch_and_receive(input, _M_DEL, _S_DEL);
			FD_CLR(sock, &r_set); FD_SET(sock, &w_set);
			input.clear();
		}
		else if (FD_ISSET(sock, &w_copy)) {
			while (true) {	//	-->
				temp_n = send(sock, output.c_str(), output.size(),0 /*MSG_DONTWAIT*/);
				if (temp_n <= 0)	break;
				output = output.substr(temp_n);
			}
			output.clear();
			FD_CLR(sock, &w_set); FD_SET(sock, &r_set);
			std::cout << "\t" << KCYN"master_process:" << KNRM << " job done successfully ->"
				<< " going back to sleep\n";
		}
	}
	exit(0);
}

void	Sockets::_enrg_env_var(std::string name, std::string value) {
	this->env_variables[ name ] = value;
}

std::string	Sockets::format_env() {
	std::string	output;
	std::map<std::string, std::string>::iterator	i = this->env_variables.begin();
	for (; i!=this->env_variables.end(); ++i)
		output.append(i->first + "=" + i->second + _S_DEL);
	return	output;
}

void	Sockets::_initiate_env_variables(char **env) {
	int		size(0);
	size_t	pos = 0;

	std::string	f_half, s_half;
	if (env)	for (;env[size]; size++);
	if (size) {
		for (int i=0; i<size; i++) {
			std::string	temp(env[i]);
			pos = temp.find("=");
			if (pos == temp.npos)	continue;
			f_half = temp.substr(0, pos);
			s_half = temp.substr(pos + 1);
			if (f_half.empty() || s_half.empty())	continue;
			this->env_variables[ f_half ] = s_half;
		}
	}
}

void	Sockets::initiate_servers(MainConfig &main_config, char **env) {
	this->_main_config = main_config;
	this->_initiate_env_variables(env);
}

bool	Sockets::initiate_master_process() {
	struct	sockaddr_un	address;
	this->socket_path = SOCKETS_PATH"/unix_quick_private_socket";
	this->check_and_remove(this->socket_path);
	std::cout << "creating cgi master process.." << std::endl;
	if ((this->cgi_controller = geta_unix_socket(address, this->socket_path)) < 0) return	false;
	if (bind(this->cgi_controller, (struct sockaddr*)&address, sizeof(address)) < 0) return	false;
	if (listen(this->cgi_controller, 5) < 0) return	false;
	std::cout << KCYN"initiating" << KNRM << " connection with master process over unix socket..\n";
	if ((this->master_PID = fork()) < 0)	return	false;
	if (!this->master_PID)	master_routine(this->socket_path);
	if ((this->master_process = ::accept(this->cgi_controller, NULL, NULL)) < 0) return	false;
	std::cout << "child created" << KGRN << " successfully" << KNRM << " and waiting for jobs in: "
		<< this->socket_path << std::endl;
	return	true;
}

bool	Sockets::update_master_state() {
	if (this->master_PID < 0 || kill(this->master_PID, 0) < 0) this->active_master = false;
	else	this->active_master = true;
	return	this->active_master;
}

void	Sockets::check_and_remove(std::string target) {
	struct	stat	output;
	if (stat(target.c_str(), &output) != 0)	return;
	std::remove(target.c_str());
}

Sockets::Sockets( void ) : _sess_id(1234), active_master(0) {
	std::srand(std::time(NULL));
	std::cout << CLR_TERM;
	this->active_master = this->initiate_master_process();
}

Sockets::~Sockets() {
	std::cout << KRED"cleaning...\n";
	std::cout << "deleting unix socket: " << KNRM
		<< this->socket_path << std::endl;
	//
	this->check_and_remove(this->socket_path);
	if (this->update_master_state()) {
		std::cout << KRED"killing master process\n" << KNRM;
		close(this->master_process);
		close(this->cgi_controller);
		kill(this->master_PID, SIGKILL);
	}
	//
	std::cout << KRED"closing active connections..\n" << KNRM;
	for (std::map<int, ServerConfig*>::iterator i=this->_fd_to_server.begin(); i!=this->_fd_to_server.end();++i)
		if (i->second && i->second->_socket == i->first)	close(i->first);
}

std::string	Sockets::execute_script(std::string input) {
	if (!this->update_master_state())
		if (!(this->active_master = this->initiate_master_process()))
			return "webserv_cgi_status=500; CGI child fork() failure";
	fd_set	r_set; FD_ZERO(&r_set);
	std::string	output;
	int	index = 0;
	for (;;) {
		index = send(this->master_process, input.c_str(), input.size(), 0);
		if (index <= 0)	break;
		input = input.substr(index);
	}
	char	buffer[CGI_PIPE_MAX_SIZE];
	struct	timeval	tv;
	std::memset(&tv, 0, sizeof(tv));
	tv.tv_sec = CGI_TIME_LIMIT;
	FD_SET(this->master_process, &r_set);
	index = select(this->master_process + 1, &r_set, NULL, NULL, &tv);
	if (index <= 0)	return "webserv_cgi_status=504; CGI operation timeout";
	else
		for (;;) {
			std::memset(buffer, 0, sizeof(buffer));
			index = recv(this->master_process, buffer, CGI_PIPE_MAX_SIZE -1, MSG_DONTWAIT);
			if (index <= 0)	break;
			else	output.append(buffer, index);
		}
	return		output;
}

void	Sockets::accept(int sock_fd) {
	struct	sockaddr	address;
	socklen_t	si = sizeof(address);
	int new_s_fd = ::accept(sock_fd, &address, &si);
	if (new_s_fd < 0)	return ;
	ServerConfig		*target = this->_fd_to_server.find(sock_fd)->second;
	this->_fd_to_server[ new_s_fd ] = target;
	this->_kqueue.SET_QUEUE(new_s_fd, EVFILT_READ, 1);
	std::cout << target->server_name << ": Accept new connection: " << KGRN << new_s_fd << KNRM << std::endl;
}

void	Sockets::recvFrom(int sock_fd) {
	ServerConfig *serv = this->_fd_to_server.find(sock_fd)->second;
	std::map<int, std::pair<Request, Response> *>::iterator	pai = serv->_requests.find(sock_fd);
	if (pai == serv->_requests.end()) {
		std::vector<ServerConfig*>	servs;
		servs.push_back(serv);
		if (serv->is_duplicated)
			for (std::map<std::string, ServerConfig*>::iterator it = this->_dup_servers.begin(); it != this->_dup_servers.end(); ++it) 
				if (it->first == serv->host+":"+serv->listen_port)	servs.push_back(it->second);
		std::pair<Request, Response>	*new_pair = new std::pair<Request, Response>;
		serv->_requests[ sock_fd ] = new_pair;
		serv->_requests[ sock_fd ]->first.set_servers( servs );
		serv->_requests[ sock_fd ]->first.set_fd( sock_fd );
		this->recvFrom( sock_fd );
		return ;
	}
	pai->second->first.recvRequest();
	if (pai->second->first.getState() == DONE || pai->second->first.getState() == ERROR) {
		char	buffer[10];
		while (recv(sock_fd, buffer, 10, MSG_DONTWAIT) >= 0);
		this->_kqueue.SET_QUEUE(sock_fd, EVFILT_READ, 0);
		this->_kqueue.SET_QUEUE(sock_fd, EVFILT_WRITE, 1);
		pai->second->second._initiate_response(&pai->second->first, *this, serv);
	}
}

void	Sockets::sendTo(int sock_fd) {
	ServerConfig *serv = this->_fd_to_server.find(sock_fd)->second;
	std::map<int, std::pair<Request, Response>* >::iterator pai = serv->_requests.find(sock_fd);
	pai->second->second.sendResponse(sock_fd, serv);
	if (pai->second->second.get_status() == DONE) {
		if (pai->second->second._connection_type == "keep-alive" && pai->second->second._has_body) {
			this->_kqueue.SET_QUEUE(sock_fd, EVFILT_WRITE, 0);
			this->_kqueue.SET_QUEUE(sock_fd, EVFILT_READ, 1);
			this->resetConn(sock_fd);
		}
		else	this->closeConn(sock_fd);
	}
}

void	Sockets::closeConn(int sock_fd) {
	this->resetConn(sock_fd);
	std::map<int, ServerConfig*>::iterator i = this->_fd_to_server.find(sock_fd);
	if (i != this->_fd_to_server.end()) {
		std::cout << "\t" << i->second->server_name << ": Close connection: "KRED << sock_fd << KNRM;
		std::cout << " , remaining sockets: " << KBGR " "<< this->_kqueue.get_current_events() << " \n"KNRM;
		this->_fd_to_server.erase(sock_fd);
	}
	this->_kqueue.SET_QUEUE(sock_fd, 0, 0);
	close(sock_fd);
}

void	Sockets::resetConn(int sock_fd) {
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

std::string	clean_up_stuff(std::string input, std::string garbage, std::string target) {
	if (input.empty()||garbage.empty()||garbage.size() != target.size())	return "";
	size_t				pos;
	for (size_t i=0; i < garbage.size(); i++)
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

std::string	_generate_random_string(std::string seed, int length) {
	std::string	output = seed;
	for (int i=0;i<length;i++) {
		std::string    r(1, static_cast<char>(std::rand() % (122 - 48) + 48 ));
		output.append( r );
	}
	return	clean_up_stuff(output, "[\\]^`:;<>=?/ ", "_____________");
}

int	__calc_new_range(int old_value, int old_min, int old_max, int new_min, int new_max) {
	if (old_max==old_min||new_min==new_max) return new_min;
	if (old_value == old_min) return new_min;
	else if (old_value>old_max) return new_max;
	return ( ((old_value-old_min)*(new_max-new_min))/(old_max-old_min)+new_min);
}

static	std::string	s_encryptor(std::string input, std::string key, int mode) {
	size_t		i=0;
	while (key.size() < input.size()) { if (i >= key.size()) i = 0; key.append(&key[i]); }
	for (i=0; i < input.size();i++) input[i]+=mode*13;
	return	input;
}

static	std::string	get_cookie_value(std::string input, std::string name)
{
	size_t	pos = input.find(name), epos;
	if (pos == input.npos) return "";
	pos += name.size();
	epos = pos;
	try {
		if (epos > input.size()) return "";
		while (epos < input.size() 
			&& input[epos] != ';' 
			&& input[epos] != ' ')
			epos += 1;
		return	input.substr(pos, epos-pos);
	}
	catch (std::exception &l)	{ return ""; }
}

void		Sockets::check_session(Response &response) {
	std::string estab_session_id = response._request->get_headers().cookie;
	std::string user_name = this->form_user_name(*response._request);
	if (user_name.empty()) {
		std::cout << "no info on client not registering session\n";
		return ;
	}
	std::string session_id = this->get_cookie(user_name, "session");
	//
	if (!estab_session_id.empty()) {
		std::string cur_session_id = get_cookie_value(estab_session_id, "session=");
		if (s_encryptor(cur_session_id, user_name, -1) == session_id) {
			std::cout << "already established/valid session: " << KCYN"[" << cur_session_id << "] <- "<<session_id<<KNRM<<std::endl;
			response.set_session_id(s_encryptor(cur_session_id, user_name, 1));
			return ;
		}
		else if (!session_id.empty()) {
			std::cout << "remembering client of their id: " << KCYN"[" << session_id << "]" << KNRM<<std::endl;
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
		std::cout << "registering a new session: " << KCYN"[" << encr_id << "] <- " << session_id <<KNRM<<std::endl;
	}
}

//////
std::string	Sockets::form_user_name(Request &request) {
	std::string host, user_agent;
	if (!request.get_headers().host.empty()) host = clean_up_stuff(request.get_headers().host, ".;=( ):", "##${_}#");
	if (!request.get_headers().user_agent.empty()) user_agent = clean_up_stuff(request.get_headers().user_agent, ".;=( ):", "##${_}#");
	return	(host+user_agent);
}


void	Sockets::set_Cookies(std::string client, std::string cookie) {
	std::map<std::string, std::string>	new_cookies;
	std::stringstream			input(cookie);
	std::string			f_half, s_half;
	char				buffer[cookie.size()];
	std::memset(buffer, 0, sizeof(buffer));

	size_t	pos = cookie.find("; ");
	while (input.read(buffer, pos))
	{
		std::string buff(buffer);	// something
		size_t eq_pos = buff.find("=");
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
	std::map<int, ServerConfig*>::iterator	it;
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
				else if (events[i].filter == EVFILT_WRITE) 
					this->sendTo(events[i].ident);
			}
	}
}

static void initAddInfo(struct addrinfo& ai) {
	memset(&ai, 0, sizeof(struct addrinfo));
	ai.ai_family = AF_UNSPEC;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_flags = AI_PASSIVE;
}

static int getAddrInfo(struct addrinfo *hints, struct addrinfo **res, mit it) {
	int					rval;

	if ((rval = getaddrinfo((*it)->host.c_str(), (*it)->listen_port.c_str(), hints, res)) != 0) {
		std::cerr << KYEL << "server: " << (*it)->host << ":" << (*it)->listen_port << " is enable to start." << KNRM << std::endl;
		return (-1);
	}
	return (0);
}

static int createSocket(struct addrinfo *res, mit it) {
	struct addrinfo		*tmp;
	int					sock;
	int					yes = 1;

	(void)it;
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
		else if (!tmp->ai_next)	return 0;
		tmp = tmp->ai_next;
	}
	return (-1);
}

void	Sockets::startServers() {
	struct addrinfo		hints;
	struct addrinfo		*res;
	int					sock;
	int					nbr = 0;
	bool					this_status(true);
	std::vector<ServerConfig* >			servers = this->_main_config.servers;

	for (mit it = servers.begin(); it != servers.end(); it++) {
		initAddInfo(hints);
		if (getAddrInfo(&hints, &res, it) < 0) continue ;
		if ((sock = createSocket(res, it)) < 0) {
			std::map<int, ServerConfig*>::iterator	i = this->_fd_to_server.begin();
			for(; i != this->_fd_to_server.end(); ++i)
				if (i->second->host == (*it)->host && i->second->listen_port == (*it)->listen_port) {
					this_status = false;
					if (i->second->server_name == (*it)->server_name)
						std::cout << KYEL"warning: " << KNRM << "same host, port, server_name, IGNORING the second\n";
					else {
						this->_dup_servers[i->second->host+":"+i->second->listen_port] = *it;
						i->second->is_duplicated = true;
					}
				}
		}
		else if (!sock) {
			std::cerr << KYEL << "server: " << (*it)->host << ":" << (*it)->listen_port << " is enable to start. socket() failure" << KNRM << std::endl;
			continue ;
		}
		freeaddrinfo(res);
		if (!this_status) { this_status = true; continue ; }
		if (listen(sock, 100) < 0)
			std::cerr << KYEL << "server: " << (*it)->host << ":" << (*it)->listen_port << " is unable to start. listen() failure" << KNRM << std::endl;
		else	std::cout << "\n[" << KGRN << (*it)->server_name << KNRM << ": " << (*it)->host << ":"
			<< (*it)->listen_port << ", " << (*it)->locations.size() << " locations " << KGRN << "STARTED successfully" << KNRM"]" << std::endl;
		(*it)->_socket = sock;
		this->_kqueue.SET_QUEUE(sock, EVFILT_READ, 1);
		this->_fd_to_server[ sock ] = *it;
		nbr++;
	}
	if (nbr == 0)
		throw std::runtime_error(KRED + std::string("No server started") + KNRM);
}

void	Sockets::run() {
	this->startServers();
	this->kqueueLoop();
}

std::string	Sockets::get_mime_type(std::string ext) { return	this->_mime.getMIME(ext); }
bool		Sockets::is_valid_mime(std::string type) {
	std::map<std::string, std::string>::iterator	i = this->_mime._mime.begin();
	for (; i!=this->_mime._mime.end(); ++i) {
		if (type.find(i->second) != type.npos)	return	true;
	}
	return	false;
}
