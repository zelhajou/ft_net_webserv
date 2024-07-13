/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:42:18 by zelhajou          #+#    #+#             */
/*   Updated: 2024/07/13 10:07:05 by beddinao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sockets.hpp"

Server	*create_demo_server(std::string name, std::string port, std::string host, std::string location, std::string root)
{
	Server		*server = new Server();

	server->_server_name = name;
	server->_port = port;
	server->_host = host;
	std::map<int, std::string>	error_pages;
	error_pages[404] = "/404.html";
	server->_error_pages = error_pages;
	server->_client_max_body_size = 100000;

	Location	location_one;
	location_one._location = location;
	location_one._root = root;
	location_one._upload_path = "/uploads/";
	location_one._index = "index.html";
	location_one._autoindex = false;
	/*std::vector<std::string>	methods;
	methods.push_back("GET");
	methods.push_back("POST");*/
	location_one._methods.push_back("GET");
	location_one._methods.push_back("POST");
	
	std::map<std::string, Location>	locations;
	locations[location] = location_one;

	server->_locations = locations;

	return	server;
}

int main(int argc, char *argv[]) {
	(void)argc;
	/*if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
		return 1;
	}*/
	/*std::string config_file = argv[1];
	ConfigParser parser;
	MainConfig main_config = parser.parse(config_file);*/
	Parser	demo("voila");
	demo._servers.push_back(create_demo_server("server_one", "8080", "localhost", "/", "/Users/beddinao/"));
	demo._servers.push_back(create_demo_server("server_two", "1234", "localhost", "/", "/Users/beddinao/"));

	try {
		// Parser parser(argv[1]); // Parse the config file
		Sockets s(demo); // Create the Sockets for all servers
		s.run(); // Start the servers using kqueue
	}
	catch (std::exception &e) {
		// If none of the servers could be started, the program should exit with a message to stderr
		std::cerr << e.what() << std::endl;
	}
    return 0;
}
