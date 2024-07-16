#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include <string>
# include <map>
# include <fstream>
# include <sys/socket.h>
# include <unistd.h>
# include <sys/stat.h>
# include "util.h"
# include "ConfigStructures.hpp"

struct	LocationNode;

class Request {
public:
	Request();
	~Request();
	Request(const Request &);
	Request &operator = (const Request &);

	void						recvRequest();
	void						POST();
	void						parse_first_line();
	void						parse_headers();
	void						parse_body();
	void						parse_uri();
	void						set_fd(int sock_fd);
	e_parser_state				getState();
	e_status					getStatus();
	void						check_uri();
	t_first_line				get_first_line();
	t_headers					get_headers();
	e_location_type				get_location_type();
	bool						is_cgi();
	bool						is_file(std::string& path);
	bool						is_directory(std::string& path);
	void						handle_cgi();
	void						handle_chunked();
	void						handle_file();
	void						handle_directory(LocationConfig* loc);
	void						handle_uri(LocationConfig* loc);
	void						handle_location(LocationConfig** loc);

	void						setStatus(e_status status);
	void						setLocation(std::map<std::string, LocationConfig> &locations);

private:
	int							_fd;
	size_t						_recv;
	e_parser_state				_state;
	e_status					_status;
	size_t						_timeout;
	bool						_has_body;
	std::ifstream				_file;
	t_first_line				_first_line;
	t_headers					_headers;
	std::string					_body;
	bool						_chunked;
	size_t						_content_length;
	std::vector<LocationNode*>	_location_tree;
	e_location_type				_location_type;
	bool						_hex;
	size_t						_chunk_size;
	std::vector<std::string>	_chunked_body;
};

# include "TrieTree.hpp"
#endif
