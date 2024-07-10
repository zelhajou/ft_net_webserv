/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 17:07:32 by zelhajou          #+#    #+#             */
/*   Updated: 2024/07/10 18:18:26 by zelhajou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP


#include "ConfigStructures.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

class ConfigParser {
public:
    MainConfig parse(const std::string& config_file);

private:
    void parseServerBlock(std::ifstream& file, MainConfig& main_config);
    void parseLocationBlock(std::ifstream& file, ServerConfig& config, const std::string& path);
    std::string trim(const std::string& str);
};

#endif