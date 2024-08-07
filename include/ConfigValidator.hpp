/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 12:52:53 by zelhajou          #+#    #+#             */
/*   Updated: 2024/08/07 17:50:37 by zelhajou         ###   ########.fr       */
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
    void checkEssentialDirectives(const ServerConfig& server);
    void validatePort(const std::string& port);
    void validateHost(const std::string& host);
    bool isValidDomainName(const std::string& domain);
    bool isValidIPAddress(const std::string& ip);
    void validateClientMaxBodySize(const std::string& size);
    
    void validateLocationConfig(const LocationConfig& location, const std::string& project_root);	
	void validateAllowedMethods(const std::vector<std::string>& methods);
	void validateRoot(const std::string& root, const std::string& project_root);
	void validateUploadStore(const std::string& upload_store, const std::string& project_root);

	void validateCGI(std::vector<std::string> add_cgi, std::string cgi_path, std::vector<std::string> cgi_allowed_methods, const std::string &location_root, const std::string &project_root);
};

#endif