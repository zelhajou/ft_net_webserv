#ifndef __UTIL_HPP__
# define __UTIL_HPP__

# include <string>

typedef struct s_first_line {
	std::string method;
	std::string uri;
	std::string version;
}				t_first_line;

typedef struct s_headers {
	std::string host;
	std::string connection;
	std::string content_type;
	std::string content_length;
	std::string transfer_encoding;
	std::string date;
	std::string accept;
	std::string location;

}				t_headers;

enum e_status {
	OK = 200,
	BAD_REQUEST = 400,
	NOT_FOUND = 404,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	RIDIRECT = 301,
	NOT_MODIFIED = 304,
	TOO_MANY_REQUESTS = 429,
	REQUEST_ENTITY_TOO_LARGE = 413,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	HTTP_VERSION_NOT_SUPPORTED = 505,
	URI_TOO_LONG = 414,
	LENGTH_REQUIRED = 411,
	REQUEST_TIMEOUT = 408,
};

enum e_parser_state {
	START,
	FIRST_LINE,
	HEADERS,
	BODY,
	DONE,
	ERROR,
};

#endif
