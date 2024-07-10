#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include <string>
# include <map>
# include <fstream>
# include "util.h"

class Request {
public:
	Request();
	~Request();

	void	POST();

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
	

};

#endif