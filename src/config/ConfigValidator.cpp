/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 13:39:35 by zelhajou          #+#    #+#             */
/*   Updated: 2024/08/06 18:38:41 by zelhajou         ###   ########.fr       */
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
    // checkDuplicateListenPorts();
    // checkDuplicateServerNames();
    for (size_t i = 0; i < config.servers.size(); ++i)
        validateServerConfig(*config.servers[i]);
}

void ConfigValidator::validateServerConfig(const ServerConfig& server)
{
	// Check for essential directives
    checkEssentialDirectives(server);

	// Validate all directives
    validatePort(server.listen_port);
    validateHost(server.host);
    validateServerName(server.server_name);
    validateErrorPages(server.error_pages);
    validateClientMaxBodySize(server.client_max_body_size);

	// Validate all locations
    std::map<std::string, LocationConfig>::const_iterator it;
    for (it = server.locations.begin(); it != server.locations.end(); ++it) {
        validateLocationConfig(it->second);
    }
}


// Check for essential directives
// ------------------------------------ //
void ConfigValidator::checkEssentialDirectives(const ServerConfig& server)
{
    if (server.listen_port.empty())
        throw std::runtime_error("Missing 'listen' directive in server block");
    if (server.server_name.empty())
        throw std::runtime_error("Missing 'server_name' directive in server block");
}
// ------------------------------------ //


// Validate all directives of a server block
// ------------------------------------ //
void ConfigValidator::validatePort(const std::string& port)
{
    std::regex port_regex("^[0-9]{1,5}$");
    if (!std::regex_match(port, port_regex) || std::stoi(port) > 65535)
        throw std::runtime_error("Invalid port: " + port);
}

// ------------------------------------ //
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
// ------------------------------------ //

void ConfigValidator::validateServerName(const std::string& server_name)
{
    if (server_name.empty())
        throw std::runtime_error("Invalid server name: " + server_name);
}


void ConfigValidator::validateErrorPages(const std::map<int, std::string>& error_pages)
{
    std::map<int, std::string>::const_iterator it;
    for (it = error_pages.begin(); it != error_pages.end(); ++it)
	{
        if (it->first < 400 || it->first > 599)
            throw std::runtime_error("Invalid error code: " + std::to_string(it->first));
        if (it->second.empty())
            throw std::runtime_error("Error page path cannot be empty for code: " + std::to_string(it->first));
        processPath(const_cast<std::string&>(it->second));
    }
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
// ------------------------------------ //
void ConfigValidator::validateLocationConfig(const LocationConfig& location)
{
    //validatePath(location.path);
    validateAllowedMethods(location.allowed_methods);
    //validatePath(location.root);
    //validatePath(location.upload_store);
    //validatePath(location.client_body_temp_path);
    //validateReturnURL(location.return_url);
}

// Validate all directives of a location block
// ------------------------------------ //

void ConfigValidator::validatePath(const std::string& path)
{
    if (path.empty())
        throw std::runtime_error("Path cannot be empty");
    processPath(const_cast<std::string&>(path));
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
void ConfigValidator::validateCGIPath(const std::string& path)
{
    if (path.empty())
        throw std::runtime_error("CGI path cannot be empty");
    processPath(const_cast<std::string&>(path));
}

void ConfigValidator::processPath(std::string& path)
{
    if (!path.empty() && path[0] == '/')
        path = project_root + path;
}

void ConfigValidator::validateReturnURL(const std::pair<e_status, std::string>& return_url)
{
    if (return_url.first < 300 || return_url.first > 399)
        throw std::runtime_error("Invalid return status code: " + std::to_string(return_url.first));
    if (return_url.second.empty())
        throw std::runtime_error("Return URL cannot be empty");
}
// ------------------------------------ //


// Check for duplicate listen ports and server names
// -------------------------------------------------- //
void ConfigValidator::checkDuplicateListenPorts()
{
    std::set<std::string> listen_ports;
    for (size_t i = 0; i < config.servers.size(); ++i)
	{
        if (listen_ports.find(config.servers[i]->listen_port) != listen_ports.end())
            throw std::runtime_error("Duplicate listen port: " + config.servers[i]->listen_port);
        listen_ports.insert(config.servers[i]->listen_port);
    }
}
void ConfigValidator::checkDuplicateServerNames()
{
    std::set<std::string> server_names;
    for (size_t i = 0; i < config.servers.size(); ++i)
	{
        if (server_names.find(config.servers[i]->server_name) != server_names.end())
            throw std::runtime_error("Duplicate server name: " + config.servers[i]->server_name);
        server_names.insert(config.servers[i]->server_name);
    }
}
// -------------------------------------------------- //