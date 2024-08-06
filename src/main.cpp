/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsobane <hsobane@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 17:42:18 by zelhajou          #+#    #+#             */
/*   Updated: 2024/07/30 04:14:03 by beddinao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sockets.hpp"

Sockets		S;

void	sig_nan(int sig_num) {
	std::cout << KCYN << "main_process:" << KNRM
		<< " received signal(" << KGRN << sig_num
		<< KNRM << ") exiting..\n";
	exit(sig_num);
}

void	leaks_fun(void) {system("leaks webserv");}

int main(int argc, char *argv[], char **env) {
	// std::cout << CLR_TERM;
	fix_up_signals(sig_nan);
	if (argc != 2) {
		std::cerr << KRED"->\tmissing configuration file"KNRM;
		std::cerr << "\tUsage: " << argv[0] << " [config_file]" << std::endl;
		return 1;
	}
	std::ifstream	file(argv[1]);
	if (!file) {
		std::cerr << KRED"Failed to open file: "KNRM << argv[1] << std::endl;
		return	1;
	}
	std::stringstream	buffer;
	buffer << file.rdbuf();
	std::string	config = buffer.str();
	Tokenizer	tokenizer(config);
	std::vector<Token>	tokens = tokenizer.tokenize();
	Parser	parser(tokens);
	try {
		MainConfig	main_config = parser.parse();
		S.initiate_servers(main_config, env);
		S.run();
	}
	catch (std::exception &e) { std::cerr << KRED << e.what() << KNRM << std::endl; }
	return 0;
}
