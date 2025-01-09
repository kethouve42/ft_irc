/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:01:18 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/09 15:21:06 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(int ac, char **av)
{
	if(ac < 3)
	{
		std::cerr << "Error: usage ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	Server server(std::atoi(av[1]), av[2]);
	server.setServerSocket();
	server.serverLoop();
}