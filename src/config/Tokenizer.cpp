/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 10:58:44 by zelhajou          #+#    #+#             */
/*   Updated: 2024/07/29 05:48:24 by beddinao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Tokenizer.hpp"
#include <cctype>

Tokenizer::Tokenizer(const std::string& input) : input(input), pos(0), line(1) {}

std::vector<Token> Tokenizer::tokenize()
{
    std::vector<Token> tokens;

	while (pos < input.length())
	{
        if (isspace(input[pos]))
		{
            if (input[pos] == '\n')
				++line;
            ++pos;
            continue;
        }
        if (input[pos] == '{')
		{
            tokens.push_back(Token(TOKEN_OPEN_BRACE, "{", line));
            ++pos;
            continue;
        }
        if (input[pos] == '}')
		{
            tokens.push_back(Token(TOKEN_CLOSE_BRACE, "}", line));
            ++pos;
            continue;
        }
        if (input[pos] == ';')
		{
            tokens.push_back(Token(TOKEN_SEMICOLON, ";", line));
            ++pos;
            continue;
        }
        if (input[pos] == '#')
		{
            while (pos < input.length() && input[pos] != '\n')
                ++pos;
            continue;
        }

        std::string word;
        while (pos < input.length() && !isspace(input[pos]) && input[pos] != '{' && input[pos] != '}' && input[pos] != ';')
            word += input[pos++];
	
        tokens.push_back(tokenizeWord(word));
    }

    return (tokens);
}

Token Tokenizer::tokenizeWord(const std::string& word)
{
    if (word == "server")
		return (Token(TOKEN_SERVER, word, line));
    if (word == "location")
		return (Token(TOKEN_LOCATION, word, line));
    if (word == "listen")
		return (Token(TOKEN_LISTEN, word, line));
    if (word == "host")
		return (Token(TOKEN_HOST, word, line));
    if (word == "server_name")
		return (Token(TOKEN_SERVER_NAME, word, line));
    if (word == "error_page")
		return (Token(TOKEN_ERROR_PAGE, word, line));
    if (word == "client_max_body_size")
		return (Token(TOKEN_CLIENT_MAX_BODY_SIZE, word, line));
    if (word == "allowed_methods")
		return (Token(TOKEN_ALLOWED_METHODS, word, line));
    if (word == "root")
		return (Token(TOKEN_ROOT, word, line));
    if (word == "index")
		return (Token(TOKEN_INDEX, word, line));
    if (word == "return")
		return (Token(TOKEN_RETURN, word, line));
    if (word == "fastcgi_pass")
		return (Token(TOKEN_FASTCGI_PASS, word, line));
    if (word == "fastcgi_index")
		return (Token(TOKEN_FASTCGI_INDEX, word, line));
    if (word == "include")
		return (Token(TOKEN_INCLUDE, word, line));
    if (word == "autoindex")
		return (Token(TOKEN_AUTOINDEX, word, line));
    if (word == "upload_store")
		return (Token(TOKEN_UPLOAD_STORE, word, line));
    if (word == "client_body_temp_path")
		return (Token(TOKEN_CLIENT_BODY_TEMP_PATH, word, line));
	if (word == "add_cgi")
		return (Token(TOKEN_ADD_CGI, word, line));
	if (word == "cgi_path")
		return (Token(TOKEN_CGI_PATH, word, line));
	if (word == "cgi_allowed_methods")
		return (Token(TOKEN_ALLOWED_CGI_METHODS, word, line));
	
    return (Token(TOKEN_STRING, word, line));
}
