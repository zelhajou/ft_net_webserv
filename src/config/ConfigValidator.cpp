/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 13:39:35 by zelhajou          #+#    #+#             */
/*   Updated: 2024/08/07 12:58:54 by zelhajou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigValidator.hpp"
#include <regex>
#include <stdexcept>
#include <set>
#include <unistd.h>
#include <pwd.h>

ConfigValidator::ConfigValidator(const MainConfig& config, const std::string& project_root)
    : config(config), project_root(project_root) {}

void ConfigValidator::validate()
{
	std::vector<std::string> errors;
    for (size_t i = 0; i < config.servers.size(); ++i)
    {
		config.servers[i]->valid = true;
		try
		{
			validateServerConfig(*config.servers[i]);
		}
   		catch (const std::runtime_error& e)
		{
            errors.push_back(e.what());
			config.servers[i]->valid = false;			
		}
	}
    if (!errors.empty())
	{
        for (size_t i = 0; i < errors.size(); ++i) {
            std::cerr << "Error: " << errors[i] << std::endl;
        }

        if (errors.size() == config.servers.size()) {
            throw std::runtime_error("All server blocks are invalid");
        }
    }
		
}

void ConfigValidator::validateServerConfig(const ServerConfig& server)
{
    checkEssentialDirectives(server);

    validatePort(server.listen_port);
    validateHost(server.host);
    validateClientMaxBodySize(server.client_max_body_size);

    std::map<std::string, LocationConfig>::const_iterator it;
    for (it = server.locations.begin(); it != server.locations.end(); ++it) {
        validateLocationConfig(it->second);
    }
}

void ConfigValidator::checkEssentialDirectives(const ServerConfig& server)
{
	if (server.listen_port.empty())
        throw std::runtime_error("Missing 'listen' directive in server block");

	if (server.locations.empty())
		throw std::runtime_error("No locations found in server block");
}

void ConfigValidator::validatePort(const std::string& port)
{
    std::regex port_regex("^[0-9]{1,5}$");
    if (!std::regex_match(port, port_regex) || std::stoi(port) > 65535)
        throw std::runtime_error("Invalid port: " + port);
}

void ConfigValidator::validateHost(const std::string& host)
{
    if (!isValidIPAddress(host) && !isValidDomainName(host))
        throw std::runtime_error("Invalid host: " + host);
}

bool ConfigValidator::isValidIPAddress(const std::string& ip)
{
    std::regex ip_regex("^(\\d{1,3}\\.){3}\\d{1,3}$");
    return std::regex_match(ip, ip_regex);
}
bool ConfigValidator::isValidDomainName(const std::string& domain)
{
    std::regex domain_regex("(^([a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,}$)|(^localhost$)");
    return std::regex_match(domain, domain_regex);
}

void ConfigValidator::validateClientMaxBodySize(const std::string& size)
{
    std::regex size_regex("^[0-9]+[KMG]?$");
	if (!size.empty())
	{
    	if (!std::regex_match(size, size_regex))
        	throw std::runtime_error("Invalid client_max_body_size: " + size);
	}
}

void ConfigValidator::validateLocationConfig(const LocationConfig& location)
{
    validateAllowedMethods(location.allowed_methods);
}


void ConfigValidator::validateAllowedMethods(const std::vector<std::string>& methods)
{
    std::vector<std::string>::const_iterator it;
    for (it = methods.begin(); it != methods.end(); ++it)
	{
        if (*it != "GET" && *it != "POST" && *it != "DELETE" && *it != "PUT")
            throw std::runtime_error("Invalid allowed method: " + *it);
    }
}
