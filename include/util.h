#ifndef __UTIL_HPP__
# define __UTIL_HPP__

# include <string>
# include <map>

# define BUFFER_SIZE 4096

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
	std::string cookie;
	std::string set_cookie;
	std::string user_agent;
}				t_headers;

enum e_status {
	OK = 200,
	BAD_REQUEST = 400,						// The request could not be understood by the server due to malformed syntax.
	NOT_FOUND = 404,						// The server has not found anything matching the Request-URI.
	INTERNAL_SERVER_ERROR = 500,			// The server encountered an unexpected condition which prevented it from fulfilling the request.
	NOT_IMPLEMENTED = 501,					// The server does not support the functionality required to fulfill the request.
	REDIRECT = 301,							// The requested resource has been assigned a new permanent URI and any future references to this resource SHOULD use one of the returned URIs.
	NOT_MODIFIED = 304,						// If the client has performed a conditional GET request and access is allowed, but the document has not been modified, the server SHOULD respond with this status code.
	TOO_MANY_REQUESTS = 429,				// The user has sent too many requests in a given amount of time.
	REQUEST_ENTITY_TOO_LARGE = 413,			// The server is refusing to process a request because the request entity is larger than the server is willing or able to process.
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,	// The server is refusing to process a request because the request header fields are too large.
	HTTP_VERSION_NOT_SUPPORTED = 505,		// The server does not support, or refuses to support, the major version of HTTP that was used in the request message.
	URI_TOO_LONG = 414,						// The server is refusing to service the request because the Request-URI is longer than the server is willing to interpret.
	LENGTH_REQUIRED = 411,					// The server refuses to accept the request without a defined Content-Length.
	REQUEST_TIMEOUT = 408,					// The server did not receive a complete request message within the time that it was prepared to wait.
	FORBIDDEN = 403,						// The server understood the request, but is refusing to fulfill it.
};

enum e_parser_state {
	FIRST_LINE,
	HEADERS,
	BODY,
	DONE,
	ERROR,
};

enum e_location_type {
	STATIC,
	CGI,
	RETURN,
	AUTOINDEX,
};

struct MIME {
public:
	MIME();
	std::string getMIME(std::string extension);
private:
	std::map<std::string, std::string> _mime;
};

#endif
