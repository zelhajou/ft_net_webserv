#include "Parser.hpp"
#include <stdexcept>
#include <sstream>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

MainConfig Parser::parse()
{
    MainConfig config;
    while (pos < tokens.size())
    {
        if (tokens[pos].getType() == TOKEN_SERVER)
            parseServerBlock(config);
        else
            reportError("Unexpected token: " + tokens[pos].getValue() + " at line " + std::to_string(tokens[pos].getLine()));
    }
    return config;
}

void Parser::parseServerBlock(MainConfig& config)
{
    expect(TOKEN_SERVER);
    expect(TOKEN_OPEN_BRACE);

    ServerConfig* server = new ServerConfig;
    while (pos < tokens.size())
    {
        if (tokens[pos].getType() == TOKEN_CLOSE_BRACE)
            break;
        else if (tokens[pos].getType() == TOKEN_LOCATION)
            parseLocationBlock(*server);
        else
            parseServerDirective(*server);
    }

    if (pos >= tokens.size() || tokens[pos].getType() != TOKEN_CLOSE_BRACE)
    {
        delete server;
        reportError("Missing closing brace for server block");
    }

    expect(TOKEN_CLOSE_BRACE);
    config.servers.push_back(server);
}

void Parser::parseLocationBlock(ServerConfig& server)
{
    expect(TOKEN_LOCATION);
    Token path = expect(TOKEN_STRING);
    expect(TOKEN_OPEN_BRACE);

 	if (server.locations.find(path.getValue()) != server.locations.end())
	{
		LocationConfig& location = server.locations[path.getValue()];
		while (pos < tokens.size())
		{
			if (tokens[pos].getType() == TOKEN_CLOSE_BRACE)
				break;
			else
				parseLocationDirective(location);
		}
	}
	else
	{
		LocationConfig location;
		location.path = path.getValue();
		while (pos < tokens.size())
		{
			if (tokens[pos].getType() == TOKEN_CLOSE_BRACE)
				break;
			else
				parseLocationDirective(location);
		}
		server.locations[path.getValue()] = location;
	}
	if (pos >= tokens.size() || tokens[pos].getType() != TOKEN_CLOSE_BRACE)
		reportError("Missing closing brace for location block");
	expect(TOKEN_CLOSE_BRACE);
}

void Parser::parseServerDirective(ServerConfig& server)
{
    Token token = tokens[pos++];
    std::vector<std::string> values;
    int line_number = token.getLine();

    while (pos < tokens.size() && tokens[pos].getType() != TOKEN_SEMICOLON && tokens[pos].getType() != TOKEN_OPEN_BRACE && tokens[pos].getType() != TOKEN_CLOSE_BRACE)
    {
        Token t = tokens[pos];
        if (t.getType() != TOKEN_STRING)
            break;
        values.push_back(t.getValue());
        ++pos;
    }

    if (pos >= tokens.size() || tokens[pos].getType() != TOKEN_SEMICOLON)
        reportError("Expected ';' after expression, but got (" + tokens[pos].getValue() + ")");
    ++pos;

    switch (token.getType())
    {
        case TOKEN_LISTEN:
		    if (values.empty())
                reportError("Missing value for 'listen' at line " + std::to_string(line_number));
            if (values.size() != 1)
                reportError("Unexpected multiple values for 'listen' at line " + std::to_string(line_number));
            server.listen_port = values[0];
            break;
        case TOKEN_HOST:
		    if (values.empty())
                reportError("Missing value for 'host' at line " + std::to_string(line_number));
            if (values.size() != 1)
                reportError("Unexpected multiple values for 'host' at line " + std::to_string(line_number));
            server.host = values[0];
            break;
        case TOKEN_SERVER_NAME:
			if (values.empty())
                reportError("Missing value for 'server_name' at line " + std::to_string(line_number));
          	if (values.size() != 1)
                reportError("Unexpected multiple values for 'server_name' at line " + std::to_string(line_number));
            server.server_name = values[0];
            break;
        case TOKEN_ERROR_PAGE:
            parseErrorPages(server, values, line_number);
            break;
        case TOKEN_CLIENT_MAX_BODY_SIZE:
			if (values.empty())
                reportError("Missing value for 'client_max_body_size' at line " + std::to_string(line_number));
            if (values.size() != 1)
                reportError("Unexpected multiple values for 'client_max_body_size' at line " + std::to_string(line_number));
            server.client_max_body_size = values[0];
            break;
        default:
            reportError("Unexpected token in server block: " + token.getValue() + " at line " + std::to_string(line_number));
    }
}

void Parser::parseLocationDirective(LocationConfig& location)
{
    Token token = tokens[pos++];
    std::vector<std::string> values;
    int line_number = token.getLine();


    while (pos < tokens.size() && tokens[pos].getType() != TOKEN_SEMICOLON && tokens[pos].getType() != TOKEN_OPEN_BRACE && tokens[pos].getType() != TOKEN_CLOSE_BRACE)
    {
        Token t = tokens[pos];
        if (t.getType() != TOKEN_STRING)
            break;
        values.push_back(t.getValue());
        ++pos;
    }

    if (pos >= tokens.size() || tokens[pos].getType() != TOKEN_SEMICOLON)
        reportError("Expected ';' after expression, but got " + tokenTypeToString(tokens[pos].getType()) + " (" + tokens[pos].getValue() + ")");
    ++pos;

    switch (token.getType())
    {
        case TOKEN_ALLOWED_METHODS:
			if (values.empty())
				reportError("Missing value for 'allowed_methods' at line " + std::to_string(line_number));
			for (size_t i = 0; i < values.size(); ++i)
			{
				if (values[i] == "GET" || values[i] == "POST" || values[i] == "DELETE" || values[i] == "PUT")
					continue;
				else
					reportError("Invalid method: " + values[i] + " at line " + std::to_string(line_number));
			}
            location.allowed_methods = values;
            break;
        case TOKEN_INDEX:
			if (values.empty())
				reportError("Missing value for 'index' at line " + std::to_string(line_number));
			if (values.size() != 1)
				reportError("Unexpected multiple values for 'index' at line " + std::to_string(line_number));
            location.index = values[0];
            break;
        case TOKEN_ROOT:
			if (values.empty())
				reportError("Missing value for 'root' at line " + std::to_string(line_number));
            if (values.size() != 1)
                reportError("Unexpected multiple values for 'root' at line " + std::to_string(line_number));
            location.root = values[0];
            break;
        case TOKEN_UPLOAD_STORE:
			if (values.empty())
				reportError("Missing value for 'upload_store' at line " + std::to_string(line_number));
            if (values.size() != 1)
                reportError("Unexpected multiple values for 'upload_store' at line " + std::to_string(line_number));
            location.upload_store = values[0];
            break;
        case TOKEN_CLIENT_BODY_TEMP_PATH:
            if (values.size() != 1)
                reportError("Unexpected multiple values for 'client_body_temp_path' at line " + std::to_string(line_number));
            location.client_body_temp_path = values[0];
            break;
        case TOKEN_RETURN:
			if (values.empty())
				reportError("Missing value for 'return' at line " + std::to_string(line_number));
			if (values.size () == 1)
				reportError("Missing value for 'return' at line " + std::to_string(line_number));
			if(values[1][0] == '"')
			{
				while (values.size() > 2 && values[2][values[2].size() - 1] == '"')
				{
					values[1] += " " + values[2];
					values.erase(values.begin() + 2);
				}
			}			
			if (values.size() != 2)
				reportError("Unexpected multiple values for 'return' at line " + std::to_string(line_number));
            location.return_url.second = values[1];
            location.return_url.first = static_cast<e_status>(std::stoi(values[0]));
            break;
        case TOKEN_AUTOINDEX:
			if (values.empty())
				reportError("Missing value for 'autoindex' at line " + std::to_string(line_number));
            if (values.size() != 1)
                reportError("Unexpected multiple values for 'autoindex' at line " + std::to_string(line_number));
            location.auto_index = (values[0] == "on");
            break;
		case TOKEN_ADD_CGI:
			if (values.empty())
				reportError("Missing value for 'add_cgi' at line " + std::to_string(line_number));
			location.add_cgi = values;
			break;
		case TOKEN_CGI_PATH:
			if (values.empty())
				reportError("Missing value for 'cgi_path' at line " + std::to_string(line_number));
			if (values.size() != 1)
				reportError("Unexpected multiple values for 'cgi_path' at line " + std::to_string(line_number));
			location.cgi_path = values[0];
			break;
		case TOKEN_CGI_ALLOWED_METHODS:
			if (values.empty())
				reportError("Missing value for 'allow_cgi_methods' at line " + std::to_string(line_number));
			for (size_t i = 0; i < values.size(); ++i)
			{
				if (values[i] == "GET" || values[i] == "POST")
					continue;
				else
					reportError("Invalid CGI method: " + values[i] + " at line " + std::to_string(line_number));
			}
			location.cgi_allowed_methods = values;
			break;
		default:
            reportError("Unexpected token in location block: " + token.getValue() + " at line " + std::to_string(line_number));
    }
}

void Parser::parseErrorPages(ServerConfig& server, const std::vector<std::string>& values, int line_number)
{
    if (values.empty())
        reportError("Expected at least one error code for 'error_page', but none found at line " + std::to_string(line_number));

    std::vector<int> codes;
    std::string path;

    for (size_t i = 0; i < values.size(); ++i) {
        if (std::isdigit(values[i][0]))
		{
            codes.push_back(std::stoi(values[i]));
        }
		else
		{
            path = values[i];
            break;
        }
    }

    if (codes.empty())
        reportError("Expected at least one error code for 'error_page', but none found at line " + std::to_string(line_number));
    if (path.empty())
        reportError("Expected error page path after error codes at line " + std::to_string(line_number));

    for (std::vector<int>::iterator it = codes.begin(); it != codes.end(); ++it)
        server.error_pages[*it] = path;
}

Token Parser::expect(TokenType expected)
{
    if (pos >= tokens.size()) {
        throw std::runtime_error("Unexpected end of input while expecting " + tokenTypeToString(expected));
    }
    if (tokens[pos].getType() != expected) {
        std::string expectedStr = tokenTypeToString(expected);
        std::string gotStr = tokenTypeToString(tokens[pos].getType());
        reportError("Expected " + expectedStr + ", but got " + gotStr + " (" + tokens[pos].getValue() + ")");
    }
    return tokens[pos++];
}


void Parser::reportError(const std::string& message, int line_number) const
{
    std::ostringstream oss;
    oss << "Error at line " << line_number << ": " << message;
    throw std::runtime_error(oss.str());
}

void Parser::reportError(const std::string& message) const
{
    if (pos < tokens.size())
        reportError(message, tokens[pos].getLine());
    else
        throw std::runtime_error("Unexpected end of input: " + message);
}

std::string Parser::tokenTypeToString(TokenType type) const
{
    switch (type) {
        case TOKEN_SERVER: return "server";
        case TOKEN_LOCATION: return "location";
        case TOKEN_LISTEN: return "listen";
        case TOKEN_HOST: return "host";
        case TOKEN_SERVER_NAME: return "server_name";
        case TOKEN_ERROR_PAGE: return "error_page";
        case TOKEN_CLIENT_MAX_BODY_SIZE: return "client_max_body_size";
        case TOKEN_ALLOWED_METHODS: return "allowed_methods";
        case TOKEN_ROOT: return "root";
        case TOKEN_INDEX: return "index";
        case TOKEN_RETURN: return "return";
        case TOKEN_FASTCGI_PASS: return "fastcgi_pass";
        case TOKEN_FASTCGI_INDEX: return "fastcgi_index";
        case TOKEN_AUTOINDEX: return "autoindex";
        case TOKEN_UPLOAD_STORE: return "upload_store";
        case TOKEN_CLIENT_BODY_TEMP_PATH: return "client_body_temp_path";
		case TOKEN_ADD_CGI: return "add_cgi";
		case TOKEN_CGI_PATH: return "cgi_path";
		case TOKEN_CGI_ALLOWED_METHODS: return "cgi_allow_methods";
        case TOKEN_OPEN_BRACE: return "{";
        case TOKEN_CLOSE_BRACE: return "}";
        case TOKEN_SEMICOLON: return ";";
        case TOKEN_STRING: return "string";
        case TOKEN_UNKNOWN: return "unknown";
        default: return "unknown";
    }
}

void ServerConfig::closeConn(int s)
{
	std::map<int, std::pair<Request, Response> *>::iterator i = this->_requests.find(s);
	if (i != this->_requests.end()) {
		delete	i->second;
	 	this->_requests.erase(i);
	}
}

ServerConfig::ServerConfig()
	: default_session_managment(false), is_duplicated(false) {}

ServerConfig::~ServerConfig() {
	std::cout << KRED << "cleaning " << KNRM << this->server_name << " configs\n";
}

MainConfig::~MainConfig()
{
	std::cout << KRED << "cleaning " << KNRM << "configuration structure..\n";
	for (std::vector<ServerConfig *>::iterator i = this->servers.begin(); i != this->servers.end(); ++i)
	 	delete *i;
}
