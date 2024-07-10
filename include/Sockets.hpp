#ifndef __SOCKETS_HPP__
# define __SOCKETS_HPP__

# include <map>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include "Server.hpp"
# include "KQueue.hpp"
# include "Parser.hpp"
# include "util.h"

typedef std::map<int, Server *>::iterator mit;

class Sockets {
public:
	Sockets();
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