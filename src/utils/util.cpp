# include "util.h"
# include "Parser.hpp"

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

void displayMainConfig(const MainConfig& main_config)
{
    for (size_t i = 0; i < main_config.servers.size(); ++i)
	{
        const ServerConfig* server = main_config.servers[i];
        std::cout << "Server Configuration:" << std::endl;
        std::cout << "  Listen Port: " << server->listen_port << std::endl;
        std::cout << "  Host: " << server->host << std::endl;
		std::cout << "  Server Name: " << server->server_name << std::endl;
        std::cout << "  Client Max Body Size: " << server->client_max_body_size << std::endl;

        std::cout << "  Error Pages:" << std::endl;
        for (std::map<int, std::string>::const_iterator ep = server->error_pages.begin(); ep != server->error_pages.end(); ++ep) {
            std::cout << "    " << ep->first << ": " << ep->second << std::endl;
        }
        std::cout << "  Locations:" << std::endl;
        for (std::map<std::string, LocationConfig>::const_iterator loc = server->locations.begin(); loc != server->locations.end(); ++loc) {
            const LocationConfig& location = loc->second;
			if (!location.path.empty())
				std::cout << "    Path: " << location.path << std::endl;
			
			if (!location.allowed_methods.empty())
			{
				std::cout << "      Allowed Methods: ";
				for (size_t k = 0; k < location.allowed_methods.size(); ++k) {
					std::cout << location.allowed_methods[k] << " ";
				}
				std::cout << std::endl;
			}
			if (!location.index.empty())
            	std::cout << "      Index: " << location.index << std::endl;
			if (!location.root.empty())
           		std::cout << "      Root: " << location.root << std::endl;
			if (!location.upload_store.empty())
            	std::cout << "      Upload Store: " << location.upload_store << std::endl;
			if (!location.client_body_temp_path.empty())
            	std::cout << "      Client Body Temp Path: " << location.client_body_temp_path << std::endl;
			if (!location.return_url.second.empty() && location.return_url.first)
            	std::cout << "      Return URL: " << location.return_url.second << " (Status: " << location.return_url.first << ")" << std::endl;
			if (!location.add_cgi.empty()){
				std::cout << "      Add CGI: ";
				for (size_t k = 0; k < location.add_cgi.size(); ++k) {
					std::cout << location.add_cgi[k] << " ";
				}
				std::cout << std::endl;
			}
			if (!location.cgi_path.empty())
				std::cout << "      CGI Path: " << location.cgi_path << std::endl;
			if (!location.cgi_allowed_methods.empty())
			{
				std::cout << "      Allowed CGI Methods: ";
				for (size_t k = 0; k < location.cgi_allowed_methods.size(); ++k) {
					std::cout << location.cgi_allowed_methods[k] << " ";
				}
			}
			if (!location.include.empty())
            	std::cout << "      Include: " << location.include << std::endl;
			if (location.auto_index)
            	std::cout << "      Auto Index: " << (location.auto_index ? "on" : "off") << std::endl;
        }
    }
}