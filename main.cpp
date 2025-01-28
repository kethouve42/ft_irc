/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:01:18 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/28 18:37:29 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <csignal>

Server* g_server = NULL;

void signalHandler (int signal)
{
	if (signal == SIGINT) //Ctrl+C
	{
		std::cout << "\nSIGINT (Ctrl+C) reçu. Fermeture du programme..." << std::endl;
		g_server->clearServ();
		std::exit(0);
	}
	else if (signal == SIGTERM) //Demande de terminaison
	{
		std::cout << "\nSIGTERM reçu. Arrêt du programme..." << std::endl;
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
		std::cout << "\nSIGQUIT (Ctrl+'\') reçu. Fermeture du programme..." << std::endl;
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
		std::cerr << "Error: usage ./ircserv <port> <password>" << std::endl;
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
	std::cout << "HERE" << std::endl;
	server.clearServ();
}