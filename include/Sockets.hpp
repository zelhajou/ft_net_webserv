#ifndef __SOCKETS_HPP__
# define __SOCKETS_HPP__

# include <map>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <iostream>
# include "Server.hpp"
# include "KQueue.hpp"
# include "Parser.hpp"
# include "util.h"
# include "ConfigStructures.hpp"

# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"
# define RESET "\033[0m"

typedef std::map<int, Server *>::iterator mit;
//typedef std::map<std::string, std::map<std::string, std::string> >::iterator cookie_it;

class Sockets {
public:
	Sockets(Parser& parser);
	~Sockets();

	void						run();
	void						startServers();
	void						accept(int);
	void						recvFrom(int);
	void						sendTo(int);
	void						closeConn(int);
	void						resetConn(int);
	void						cleanUp();
	void						kqueueLoop();
	std::string					get_mime_type(std::string);
	int						get_current_sessions();
	int						set_current_sessions(int);
	//
	void							set_Cookies(std::string, std::string);
	std::map<std::string, std::map<std::string, std::string> >::iterator	&get_Cookies(std::string);
	std::string						get_cookie(std::string, std::string);
	std::string						get_client(std::string, std::string);
	std::string						form_user_name(Request &);
	//std::string						xor_encryptor(std::string, std::string);
	void							check_session(Response &);

private:
	MIME						_mime;
	Parser&						_parser;
	KQueue						_kqueue;
	std::map<int, Server *>				_fd_to_server;
	std::map<std::string, std::map<std::string, std::string> >	_Cookies;
	int						_sessions;
};

#endif
