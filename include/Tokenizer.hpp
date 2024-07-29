/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 10:54:23 by zelhajou          #+#    #+#             */
/*   Updated: 2024/07/26 20:41:41 by zelhajou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>

enum TokenType
{
    TOKEN_SERVER,
    TOKEN_LOCATION,
    TOKEN_LISTEN,
    TOKEN_HOST,
    TOKEN_SERVER_NAME,
    TOKEN_ERROR_PAGE,
    TOKEN_CLIENT_MAX_BODY_SIZE,
    TOKEN_ALLOWED_METHODS,
    TOKEN_ROOT,
    TOKEN_INDEX,
    TOKEN_RETURN,
    TOKEN_FASTCGI_PASS,
    TOKEN_FASTCGI_INDEX,
    TOKEN_INCLUDE,
    TOKEN_AUTOINDEX,
    TOKEN_UPLOAD_STORE,
    TOKEN_CLIENT_BODY_TEMP_PATH,
	TOKEN_ADD_CGI,
	TOKEN_CGI_PATH,
	TOKEN_ALLOWED_CGI_METHODS,
    TOKEN_OPEN_BRACE,
    TOKEN_CLOSE_BRACE,
    TOKEN_SEMICOLON,
    TOKEN_STRING,
    TOKEN_UNKNOWN
};

class Token
{
	private:
		TokenType	type;
		std::string	value;
		int			line;

	public:
		Token() : type(TOKEN_UNKNOWN), value(""), line(0) {}
		Token(TokenType type, const std::string& value, int line)
		{
			this->type = type;
			this->value = value;
			this->line = line;
		}
		
		TokenType getType() const
		{
			return type;
		}
		const std::string& getValue() const
		{
			return value;
		}
		int getLine() const
		{
			return line;
		}
};

class Tokenizer
{
	public:
		std::vector<Token> tokenize();
		Tokenizer(const std::string& input);

	private:
		std::string	input;
		size_t		pos;
		int			line;
		Token tokenizeWord(const std::string& word);

};

#endif
