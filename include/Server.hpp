#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include "Location.hpp"
# include "CGI.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include <string>
# include <vector>
# include <map>

class Server {
public:
	Server();
	~Server();

	std::string		getHost() const;
	std::string		getPort() const;

	void			closeConn(int fd);
	/*Parse Methods Start */

	/*Parse Methods End */
	

private:
	/*CONFIG START*/
	std::vector<Location>		_locations;
	std::string			_server_name;
	std::string			_port;
	std::string			_host;
	std::map<int, std::string>		_error_pages;
	size_t				_client_max_body_size;
	/*CONFIG END*/
public:
	int						_socket;
	std::map<int, std::pair<Request, Response> >		_requests;
	std::map<int, CGI>					_cgi;
	
};

#endif
