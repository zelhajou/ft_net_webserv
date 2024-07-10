#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include <string>
# include <map>
# include <fstream>
# include <sys/socket.h>
# include <unistd.h>
# include "util.h"

class Request {
public:
	Request();
	~Request();

	void	recvRequest();
	void	POST();
	void	parse_first_line();
	void	parse_headers();
	void	parse_body();
	e_parses_state	get_parser_status( void );
	e_status		get_status_code( void );
private:
	e_parser_stat	_state;
	e_status		_status;
	bool				_has_body;
	int					_fd;
	std::ifstream		_file;
	size_t				_recv;
	t_first_line		_first_line;
	t_headers			_headers;
	std::string			_body;
	size_t				_timeout;
	bool				_chunked;
	size_t				_content_length;
};

#endif
