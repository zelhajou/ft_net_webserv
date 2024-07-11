#ifndef __LOCATION_HPP__
# define __LOCATION_HPP__

# include <vector>
# include <string>

class Location {
public:
	Location();
	virtual ~Location();

	std::string					getMethod();
	std::string					getLocation();
	std::string					getRoot();
	std::string					getUploadPath();
	std::string					getIndex();
	bool						getAutoindex();
	int							getReturnCode();

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