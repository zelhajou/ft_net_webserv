#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include "Server.hpp"
# include <string>
# include <vector>

typedef std::vector<Server *>::iterator sit;

class Parser {
public:
	Parser(std::string config_file);
	~Parser();

	std::vector<Server *>		getServers() const;

//private:
	std::vector<Server *>		_servers;

};

#endif
