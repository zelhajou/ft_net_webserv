#ifndef __LOCATION_HPP__
# define __LOCATION_HPP__

# include "CGI.hpp"
# include <vector>
# include <string>

class Location {
public:
	Location();
	~Location();

protected:
	std::string					_location;
	std::string					_root;
	std::string					_upload_path;
	std::vector<std::string>	_methods;
	std::string					_index;
	bool						_autoindex;
	int							_return_code;

};

#endif