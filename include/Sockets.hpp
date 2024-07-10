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

# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"
# define RESET "\033[0m"

typedef std::map<int, Server *>::iterator mit;

class Sockets {
public:
	Sockets(Parser& parser);
	~Sockets();

	void						run();
	void						startServers();
	void						accept();
	void						recvFrom();
	void						sendTo();
	void						closeCon();
	void						cleanUp();
	void						kqueueLoop();

private:
	MIME						_mime;
	Parser&						_parser;
	KQueue						_kqueue;
	std::map<int, Server *>		_fd_to_server;


};

#endif