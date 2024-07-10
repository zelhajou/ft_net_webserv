#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include <string>
# include <fstream>
# include "util.h"
# include "Request.hpp"

class	Response {
public:
	Response();
	~Response();

	void		_initiate_response(Request &req);
	void		sendResponse(int, Server*);
	e_parser_status	form_headers(Server*);
	size_t		get_file_size();
	e_parser_status	get_status();
	void		GET();
	void		DELETE();

private:
	int			_fd;
	std::ifstream		_file;
	size_t			_file_size;
	std::vector<size_t>		_sent;
	Request			&_request;
	e_parser_status		status;
	std::string		header;
	bool			_has_body;
	//t_first_line		_first_line;
	//t_headers		_headers;
	//std::string		_body;
	//e_status		_status;
};

#endif
