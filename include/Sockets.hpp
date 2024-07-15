#ifndef __SOCKETS_HPP__
# define __SOCKETS_HPP__

# include <map>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <unistd.h>
# include <sstream>
# include <fcntl.h>
# include <iostream>
# include <cstdlib>
# include <ctime>
# include "KQueue.hpp"
# include "util.h"
# include "ConfigParser.hpp"
//# include "ConfigStructures.hpp"

# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"
# define RESET "\033[0m"

# define KNRM  "\x1B[0m"
# define KRED  "\x1B[31m"
# define KGRN  "\x1B[32m"
# define KYEL  "\x1B[33m"
# define KBLU  "\x1B[34m"
# define KMAG  "\x1B[35m"
# define KCYN  "\x1B[36m"
# define KWHT  "\x1B[37m"

class	Response;
class	Request;
struct	ServerConfig;
struct	MainConfig;
struct	LocationConfig;

typedef std::vector<ServerConfig *>::iterator mit;

class Sockets {
public:
	Sockets(MainConfig&);
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
	size_t						get_sess_id();
	void						incr_sess_id();
	//
	void							set_Cookies(std::string, std::string);
	std::map<std::string, std::map<std::string, std::string> >::iterator	get_Cookies(std::string);
	std::string						get_cookie(std::string, std::string);
	std::string						get_client(std::string, std::string);
	std::string						form_user_name(Request &);
	void							check_session(Response &);
private:
	MIME						_mime;
	MainConfig&					_main_config;
	KQueue						_kqueue;
	std::map<int, ServerConfig *>				_fd_to_server;
	std::map<std::string, std::map<std::string, std::string> >	_Cookies;
	size_t						_sess_id;
};

# include "Response.hpp"
# include "Request.hpp"
# include "ConfigStructures.hpp"

#endif
