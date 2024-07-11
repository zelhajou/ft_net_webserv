#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include <string>
# include <map>
# include <fstream>
# include <sys/socket.h>
# include <unistd.h>
# include <sys/stat.h>
# include "util.h"
# include "Location.hpp"
# include "TrieTree.hpp"

class Request {
public:
	Request(std::map<std::string, Location>& locations);
	~Request();

	void				recvRequest();
	void				POST();
	void				parse_first_line();
	void				parse_headers();
	void				parse_body();
	void				parse_uri();
	e_parser_state		getState();
	e_status			getStatus();
	void				check_uri();
	t_first_line		get_first_line();
	t_headers			get_headers();
private:
	e_parser_state				_state;
	e_status					_status;
	bool						_has_body;
	int							_fd;
	std::ifstream				_file;
	size_t						_recv;
	t_first_line				_first_line;
	t_headers					_headers;
	std::string					_body;
	size_t						_timeout;
	bool						_chunked;
	size_t						_content_length;
	std::vector<LocationNode*>	_location_tree;
	std::string					_return_string;
};

#endif
