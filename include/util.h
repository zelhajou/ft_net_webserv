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

struct MIME {
public:
	MIME();
	std::string getMIME(std::string extension);
private:
	std::map<std::string, std::string> _mime;
};

MIME::MIME() {
	_mime[".aac"] = "audio/aac";
	_mime[".abw"] = "application/x-abiword";
	_mime[".arc"] = "application/x-freearc";
	_mime[".avi"] = "video/x-msvideo";
	_mime[".azw"] = "application/vnd.amazon.ebook";
	_mime[".bin"] = "application/octet-stream";
	_mime[".bmp"] = "image/bmp";
	_mime[".bz"] = "application/x-bzip";
	_mime[".bz2"] = "application/x-bzip2";
	_mime[".csh"] = "application/x-csh";
	_mime[".css"] = "text/css";
	_mime[".csv"] = "text/csv";
	_mime[".doc"] = "application/msword";
	_mime[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_mime[".eot"] = "application/vnd.ms-fontobject";
	_mime[".epub"] = "application/epub+zip";
	_mime[".gz"] = "application/gzip";
	_mime[".gif"] = "image/gif";
	_mime[".htm"] = "text/html";
	_mime[".html"] = "text/html";
	_mime[".ico"] = "image/vnd.microsoft.icon";
	_mime[".ics"] = "text/calendar";
	_mime[".jar"] = "application/java-archive";
	_mime[".jpeg"] = "image/jpeg";
	_mime[".jpg"] = "image/jpeg";
	_mime[".js"] = "text/javascript";
	_mime[".json"] = "application/json";
	_mime[".jsonld"] = "application/ld+json";
	_mime[".mid"] = "audio/midi";
	_mime[".midi"] = "audio/midi";
	_mime[".mjs"] = "text/javascript";
	_mime[".mp3"] = "audio/mpeg";
	_mime[".mpeg"] = "video/mpeg";
	_mime[".mpkg"] = "application/vnd.apple.installer+xml";
	_mime[".odp"] = "application/vnd.oasis.opendocument.presentation";
	_mime[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_mime[".odt"] = "application/vnd.oasis.opendocument.text";
	_mime[".oga"] = "audio/ogg";
	_mime[".ogv"] = "video/ogg";
	_mime[".ogx"] = "application/ogg";
	_mime[".opus"] = "audio/ogg";
	_mime[".otf"] = "font/otf";
	_mime[".png"] = "image/png";
	_mime[".pdf"] = "application/pdf";
	_mime[".php"] = "application/x-httpd-php";
	_mime[".ppt"] = "application/vnd.ms-powerpoint";
	_mime[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_mime[".rar"] = "application/vnd.rar";
	_mime[".rtf"] = "application/rtf";
	_mime[".sh"] = "application/x-sh";
	_mime[".svg"] = "image/svg+xml";
	_mime[".swf"] = "application/x-shockwave-flash";
	_mime[".tar"] = "application/x-tar";
	_mime[".tif"] = "image/tiff";
	_mime[".tiff"] = "image/tiff";
	_mime[".ts"] = "video/mp2t";
	_mime[".ttf"] = "font/ttf";
	_mime[".txt"] = "text/plain";
	_mime[".vsd"] = "application/vnd.visio";
	_mime[".wav"] = "audio/wav";
	_mime[".weba"] = "audio/webm";
	_mime[".webm"] = "video/webm";
	_mime[".webp"] = "image/webp";
	_mime[".woff"] = "font/woff";
	_mime[".woff2"] = "font/woff2";
	_mime[".xhtml"] = "application/xhtml+xml";
	_mime[".xls"] = "application/vnd.ms-excel";
	_mime[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_mime[".xml"] = "text/xml";
	_mime[".mp4"] = "video/mp4";
	_mime[".zip"] = "application/zip";
	_mime[".7z"] = "application/x-7z-compressed";
}

std::string MIME::getMIME(std::string extension) {
	if (_mime.find(extension) != _mime.end())
		return _mime[extension];
	return "application/octet-stream";
}

#endif
