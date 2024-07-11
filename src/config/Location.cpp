#include "Location.hpp"

Location::Location() : _location(""), _root(""), _upload_path(""), _methods(), _index(""), _autoindex(false), _return_code(0) {}

Location::~Location() {}

std::string		Location::getMethod() { return this->_methods; }
std::string		Location::getLocation() { return this->_location; }
std::string		Location::getRoot() { return this->_root; }
std::string		Location::getUploadPath() { return this->_upload_path; }
std::string		Location::getIndex() { return this->_index; }
bool			Location::getAutoindex() { return this->_autoindex; }
int				Location::getReturnCode() { return this->_return_code; }
