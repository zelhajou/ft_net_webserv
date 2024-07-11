#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include <string>
# include <fstream>
# include "util.h"
# include "Request.hpp"
# include "Sockets.hpp"

# define	FILE_READ_BUFFER_SIZE	100
# define	CRLF	"\r\n"

class	Response {
public:
	Response();
	~Response();

	void		_initiate_response(Request &, Sockets &);
	void		sendResponse(int, Server*);
	size_t		form_headers(Server*);
	e_parser_state	get_status();
	size_t		get_file_size();
	/*void		GET();
	void		DELETE();*/

private:
	int			_fd;
	std::ifstream		_file;
	size_t			_file_size;
	std::string		_file_type;
	std::vector<size_t>		_sent;
	Request			&_request;
	e_parser_state		status;
	std::string		header;
	bool			_has_body;
	//t_first_line		_first_line;
	//t_headers		_headers;
	//std::string		_body;
	//e_status		_status;
};

#endif
