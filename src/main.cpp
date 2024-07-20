/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsobane <hsobane@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:42:18 by zelhajou          #+#    #+#             */
/*   Updated: 2024/07/19 22:59:35 by beddinao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sockets.hpp"

Sockets		S;

///		example of expected config components
ServerConfig	*demo_server(std::string port, std::string host, std::string server_name,
		std::string l_path, std::string l_index, std::string l_root, bool l_auto_index) {

	ServerConfig			*server = new ServerConfig;
	server->listen_port = port;
	server->host = host;
	server->server_name = server_name;
	std::map<int, std::string>	error_pages;
	error_pages[INTERNAL_SERVER_ERROR] = CONFIG_PATH"/html_default_error_files/500.html";
	error_pages[BAD_REQUEST] = CONFIG_PATH"/html_default_error_files/400.html";
	server->error_pages = error_pages;

	LocationConfig			_location_one;
	_location_one.path = l_path;
	_location_one.root = l_root;
	_location_one.index = l_index;
	_location_one.auto_index = l_auto_index;
	std::vector<std::string>	allowed_methods;
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");
	_location_one.allowed_methods = allowed_methods;
	_location_one.upload_store = "";
	_location_one.client_body_temp_path = "";
	std::pair<e_status, std::string>	return_url;
	return_url.first = NONE;
	return_url.second = "";
	_location_one.return_url = return_url;
	_location_one.fastcgi_pass = "";
	_location_one.fastcgi_index = "";
 	_location_one.include = "";

	std::map<std::string, LocationConfig>	locations;
	locations[ l_path ] = _location_one;
	server->locations = locations;

	return	server;
}

void	sig_nan(int sig_num) {
	std::cout << KCYN << "main_process:" << KNRM
		<< " received signal(" << KGRN << sig_num
		<< KNRM << ") exiting..\n";
	exit(sig_num);
}

int main(int argc, char *argv[]) {
	fix_up_signals(sig_nan);
	/*(void)argc;
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
		return 1;
	}
	std::string config_file = argv[1];
	ConfigParser parser;
	MainConfig main_config = parser.parse(config_file);*/

	//	demo play
	std::vector<ServerConfig*>	servers;
	servers.push_back(demo_server("8080", "localhost", "server_one", "/", "", SERVER_ALL_ROOT, true));
	servers.push_back(demo_server("1234", "localhost", "server_two", "/", "", SERVER_ALL_ROOT"/server2", true));
	MainConfig	main_config;
	main_config.servers = servers;
	//

	try {
		// Parser parser(argv[1]); // Parse the config file
		S.initiate_servers(main_config); // Create the Sockets for all servers
		S.run(); // Start the servers using kqueue
	}
	catch (std::exception &e) {
		// If none of the servers could be started, the program should exit with a message to stderr
		std::cerr << e.what() << std::endl;
	}
    return 0;
}
