/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStructures.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 14:11:47 by zelhajou          #+#    #+#             */
/*   Updated: 2024/07/18 23:56:53 by beddinao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSTRUCTURES_HPP
# define CONFIGSTRUCTURES_HPP

//# include "Sockets.hpp"
# include "util.h"
# include <string>
# include <vector>
# include <iostream>
# include <map>

class	Request;
class	Response;

// Structure to represent a location block in the configuration
struct LocationConfig
{
	std::string					path;
	std::vector<std::string>	allowed_methods;
	std::string					index;
    std::string					root;
	std::string					upload_store;
	std::string					client_body_temp_path;
	std::pair<e_status, std::string>				return_url;
    std::string					fastcgi_pass;
    std::string					fastcgi_index;
    std::string					include;
	bool						auto_index;
};

// Structure to represent the main server configuration
struct ServerConfig {
	std::string									listen_port;
	std::string								host;
	std::string								server_name;
	std::map<int, std::string>						error_pages;
    std::string								client_max_body_size;
    std::map<std::string, LocationConfig>					locations;
    int				_socket;
    std::map<int, std::pair<Request, Response> >				_requests;
    //
    void	closeConn(int);	// remove entry from _requests
	public:
		~ServerConfig();
};

// Main configuration structure to hold multiple server blocks
struct MainConfig {
	std::vector<ServerConfig *>	servers;
	public:
		~MainConfig();
};

# include "Request.hpp"
# include "Response.hpp"

#endif
