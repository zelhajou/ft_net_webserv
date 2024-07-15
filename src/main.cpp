/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelhajou <zelhajou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:42:18 by zelhajou          #+#    #+#             */
/*   Updated: 2024/07/15 02:51:36 by beddinao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sockets.hpp"

int main(int argc, char *argv[]) {
	(void)argc;
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
		return 1;
	}
	std::string config_file = argv[1];
	ConfigParser parser;
	MainConfig main_config = parser.parse(config_file);

	try {
		// Parser parser(argv[1]); // Parse the config file
		Sockets s(main_config); // Create the Sockets for all servers
		s.run(); // Start the servers using kqueue
	}
	catch (std::exception &e) {
		// If none of the servers could be started, the program should exit with a message to stderr
		std::cerr << e.what() << std::endl;
	}
    return 0;
}
