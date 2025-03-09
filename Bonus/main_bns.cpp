/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:01:18 by kethouve          #+#    #+#             */
/*   Updated: 2025/03/07 22:11:04 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Server.hpp"
#include <csignal>

Server* g_server = NULL;

void signalHandler (int signal)
{
	if (signal == SIGINT) //Ctrl+C
	{
		std::cout << RED << "\nSIGINT (Ctrl+C) reçu. Fermeture du programme..." << RESET << std::endl;
		g_server->clearServ();
		std::exit(0);
	}
	else if (signal == SIGTERM) //Demande de terminaison
	{
		std::cout << RED << "\nSIGTERM reçu. Arrêt du programme..." << RESET << std::endl;
		g_server->clearServ();
		std::exit(0);
	}
	else if (signal == SIGHUP) // Fermeture terminal
	{
		g_server->clearServ();
		std::exit(0);
	}
	else if (signal == SIGQUIT) //Ctrl+'\'
	{
		std::cout << RED << "\nSIGQUIT (Ctrl+'\') reçu. Fermeture du programme..." << RESET << std::endl;
		g_server->clearServ();
		std::exit(0);
	}
	else if (signal == SIGTSTP) //Ctrl+Z
	{}
	(void)signal;
}

int	main(int ac, char **av)
{
	if(ac < 3)
	{
		std::cerr << RED << "Error: usage ./ircserv <port> <password>" << RESET << std::endl;
		return 1;
	}
	std::signal(SIGINT, signalHandler);
	std::signal(SIGTERM, signalHandler);
	std::signal(SIGHUP, signalHandler);
	std::signal(SIGQUIT, signalHandler);
	std::signal(SIGTSTP, signalHandler);
	Server server(std::atoi(av[1]), av[2]);
	g_server = &server;
	server.setServerSocket();
	server.serverLoop();
	server.clearServ();
}