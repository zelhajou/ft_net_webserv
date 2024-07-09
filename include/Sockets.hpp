#ifndef __SOCKETS_HPP__
# define __SOCKETS_HPP__

# include <map>
# include "Server.hpp"
# include "KQueue.hpp"
# include "Parser.hpp"

class Sockets {
public:
	Sockets();
	~Sockets();

	void						start();

private:
	Parser&						_parser;
	KQueue						_kqueue;
	std::map<int, Server *>		_fd_to_server;


};

#endif