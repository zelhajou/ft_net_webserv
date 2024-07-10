#include "Parser.hpp"

Parser::Parser(std::string config_file) {}

Parser::~Parser() {}

std::vector<Server *>	Parser::getServers() const {
	return (this->_servers);
}