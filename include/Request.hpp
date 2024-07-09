#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include <string>
# include <map>
# include "util.h"

class Request {
public:
	Request();
	~Request();

private:
	t_first_line		_first_line;
	t_headers			_headers;
	std::string			_body;
	e_parser_state		_state;
	

};

#endif