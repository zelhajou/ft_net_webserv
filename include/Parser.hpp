/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 17:07:32 by zelhajou          #+#    #+#             */
/*   Updated: 2024/08/06 11:35:17 by zelhajou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include "Tokenizer.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include "ConfigStructures.hpp"


class Parser
{
	public:
		Parser(const std::vector<Token>& tokens);
		MainConfig parse();
		void displayMainConfig(const MainConfig &main_config);

	private:
		void parseServerBlock(MainConfig& config);
		void parseLocationBlock(ServerConfig& server);
		void parseServerDirective(ServerConfig& server);
		void parseLocationDirective(LocationConfig& location);
		void parseErrorPages(ServerConfig& server, const std::vector<std::string>& values, int line_number);
		Token expect(TokenType expected);
		void reportError(const std::string& message, int line_number) const;
		void reportError(const std::string& message) const;
		std::string tokenTypeToString(TokenType type) const;
		void expectSemicolon();

		std::vector<Token> tokens;
		size_t pos;
};

#endif 
