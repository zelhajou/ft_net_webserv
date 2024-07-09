#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include "Location.hpp"
# include "CGI.hpp"
# include <string>
# include <vector>
# include <map>

class Server {
public:
	Server();
	~Server();

	/*Parse Methods Start */

	/*Parse Methods End */
	

private:
	/*CONFIG START*/
	std::vector<Location>		_locations;
	std::string					_server_name;
	int							_port;
	std::string					_host;
	std::map<int, std::string>	_error_pages;
	size_t						_client_max_body_size;
	/*CONFIG END*/

};

#endif