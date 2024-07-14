#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include <string>
# include <fstream>
# include "util.h"
# include "Request.hpp"
# include "Sockets.hpp"

# define	FILE_READ_BUFFER_SIZE	100
# define  DEFAULT_ERROR_PATH		"/Users/beddinao/cursus-0/webserv/42cursus-webserv/err_status_html/"
# define	CRLF			"\r\n"

class	Response {
public:
	Response();
	Response(const Response &);
	Response	&operator = (const Response &);
	~Response();

	void		_initiate_response(Request *, Sockets &, Server*);
	void		sendResponse(int, Server*);
	size_t		form_headers(Server*);
	e_parser_state	get_status();
	size_t		get_file_size();
	void		set_session_id(std::string);
	bool		_new_session;
	std::string	_connection_type;
//private:
	std::ifstream		_file;
	size_t			_file_size;
	std::string		_file_type;
	std::string		_session_id;
	std::vector<size_t>		_sent;
	Request			*_request;
	e_parser_state		status;
	std::string		header;
	bool			_has_body;
};

#endif
