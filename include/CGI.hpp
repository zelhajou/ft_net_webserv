#ifndef	__CGI_HPP__
# define __CGI_HPP__

# include <string>
# include <vector>
# include "Location.hpp"


class CGI: public Location{
public:
	CGI();
	~CGI();

private:
	std::string					_cgi_path;
	std::string					_cgi_extension;
};

#endif