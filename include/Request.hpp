#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include <string>
# include <map>
# include <fstream>
# include <sys/socket.h>
# include <unistd.h>
# include "util.h"
# include "Location.hpp"

class Request {
public:
	Request(Location& location);
	~Request();

	void	recvRequest();
	void	POST();
	void	parse_first_line();
	void	parse_headers();
	void	parse_body();
	void	check_uri();

private:
	int					_fd;
	std::ifstream		_file;
	size_t				_recv;
	t_first_line		_first_line;
	t_headers			_headers;
	std::string			_body;
	e_parser_state		_state;
	e_status			_status;
	size_t				_timeout;
	bool				_has_body;
	bool				_chunked;
	size_t				_content_length;
	Location&			_location;
};

#endif