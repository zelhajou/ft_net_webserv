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
# include "TrieTree.hpp"

# define URI_MAX_LENGTH 2048
# define HEADER_MAX_LENGTH 8192

/* ERROR MESSAGES */
# define FL_TL			"First line too long"
# define NO_SP_FL		"No SP in first line"
# define NO_SP_HD		"No SP in headers"
# define INV_MTH		"Invalid method"
# define INV_URI		"Invalid URI"
# define INV_VER		"Invalid version"
# define LOC_NF			"Location not found"
# define NOT_FND		"Not found"
# define NO_CL_HD		"Bad header field"
# define MIS_HOST		"Missing host header"
# define LEN_REQ		"Length required"
# define LEN_NOT_MATCH	"Length does not match"
# define INV_BD			"Invalid body"
# define INV_LOC_FILE	"Invalid location file"
# define INV_LOC_DIR	"Invalid location directory"
# define CANT_DELL		"Can't delete file/directory"


struct	LocationNode;

class Request {
public:
	Request();
	~Request();
	Request(const Request &);
	Request &operator = (const Request &);

	void						recvRequest();
	void						parse_first_line();
	void						parse_headers();
	void						parse_body();
	void						parse_uri();
	void						set_fd(int sock_fd);
	e_parser_state				getState();
	e_status					getStatus();
	t_first_line				get_first_line();
	t_headers					get_headers();
	e_location_type				get_location_type();
	bool						is_cgi();
	bool						is_file(std::string& path);
	bool						is_directory(std::string& path, int flag);
	void						handle_cgi();
	void						handle_chunked();
	void						handle_centent_length();
	void						handle_file();
	void						handle_directory(LocationConfig* loc);
	void						handle_uri(LocationConfig* loc);
	void						handle_location(LocationConfig** loc);
	void						extract_query_string();
	void						parse_query_string();
	void						parse_multipart();
	void						handle_post_file(std::string section);
	void						handle_post_fields(std::string section);
	void						set_first_line();
	void						set_headers();
	void						set_body();
	void						set_method();
	void						set_content_type();
	void						set_transfer_encoding();
	void						setStatus(e_status status);
	void						setLocation(std::map<std::string, LocationConfig> &locations);
	void						setRequestState(std::string msg, e_status status, e_parser_state state);


public:
	int							_fd;
	size_t						_total_body_size;
	e_parser_state				_state;
	e_status					_status;
	t_request					_request;
	bool						_chunked;
	std::vector<LocationNode*>	_location_tree;
	e_location_type				_location_type;
	size_t						_chunk_size;
	std::string					_unchunked_body;
	t_vpair						_query_string;
	std::vector<t_post_body>	_post_body;
	std::string					_request_buffer;
	e_method					_method;
};

#endif
