#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include "Server.hpp"
# include <string>
# include <vector>

class Parser {
public:
	Parser(std::string config_file);
	~Parser();

private:
	std::vector<Server *>		_servers;

};

#endif