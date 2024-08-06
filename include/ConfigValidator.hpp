/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 12:52:53 by zelhajou          #+#    #+#             */
/*   Updated: 2024/08/06 18:37:38 by zelhajou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGVALIDATOR_HPP
#define CONFIGVALIDATOR_HPP

#include "ConfigStructures.hpp"
#include <stdexcept>
#include <regex>
#include <set>

class ConfigValidator {
public:
    ConfigValidator(const MainConfig& config, const std::string& project_root);

    void validate();

private:
    const MainConfig& config;
    const std::string project_root;

    void validateServerConfig(const ServerConfig& server);
    void validateLocationConfig(const LocationConfig& location);

    void checkEssentialDirectives(const ServerConfig& server);
    void validatePort(const std::string& port);
    void validateHost(const std::string& host);
    void validateServerName(const std::string& server_name);
    void validateErrorPages(const std::map<int, std::string>& error_pages);
    void validateClientMaxBodySize(const std::string& size);
    
	
	void validateAllowedMethods(const std::vector<std::string>& methods);
    void validatePath(const std::string& path);
    void validateCGIPath(const std::string& path);
    void validateReturnURL(const std::pair<e_status, std::string>& return_url);

    void checkDuplicateListenPorts();
    void checkDuplicateServerNames();

    bool isValidDomainName(const std::string& domain);
    bool isValidIPAddress(const std::string& ip);

    void processPath(std::string& path);
};

#endif