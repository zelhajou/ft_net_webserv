#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include <string>
# include <fstream>
# include "util.h"
# include "Request.hpp"

# define BUFFER_SIZE 4096

class	Response {
public:
	Response(Request &req);
	~Response();

	void	setRequest(Request &req);
	void	sendResponse();
	void	GET();
	void	DELETE();

private:
	int					_fd;
	std::ifstream		_file;
	size_t				_sent;
	Request				&_req;
	t_first_line		_first_line;
	t_headers			_headers;
	std::string			_body;
	e_status			_status;
};

#endif