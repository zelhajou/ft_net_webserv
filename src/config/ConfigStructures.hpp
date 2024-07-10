/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStructures.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 14:11:47 by zelhajou          #+#    #+#             */
/*   Updated: 2024/07/10 17:36:05 by zelhajou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSTRUCTURES_HPP
# define CONFIGSTRUCTURES_HPP

# include <string>
# include <vector>
# include <map>
# include <unordered_map>

// Structure to represent a location block in the configuration
struct LocationConfig
{
	std::string					path;
	std::vector<std::string>	allowed_methods;
	std::string					index;
    std::string					root;
	std::string					upload_store;
	std::string					client_body_temp_path;
	std::string					return_url;
    std::string					fastcgi_pass;
    std::string					fastcgi_index;
    std::string					include;
	bool auto_index = false;
};

// Structure to represent the main server configuration
struct ServerConfig {
    int												listen_port;
	std::string										host;
	std::string										server_name;
	std::unordered_map<int std::string>				error_pages;
    std::string										client_max_body_size;
    std::unordered_map<std::string, LocationConfig>	locations;
};

// Main configuration structure to hold multiple server blocks
struct MainConfig {
	std::vector<ServerConfig>	servers;
};

#endif