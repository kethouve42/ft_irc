/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 13:52:44 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/09 15:52:47 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <sstream>
#include <map>
#include <climits>

#include "User.hpp"
#include "Channels.hpp"

#define BUFFER_SIZE 1024

class User;
class Channels;

class Server
{
	private:
		std::string _serverPass;
		sockaddr_in _serverAddr;
		int			_serverPort;
		int			_serverfd;
		std::map<int, User> _user;
		std::map<std::string, Channels> _channels;
		Server();
	public:
		Server(int port, std::string pass);
		~Server();
		void	setServerSocket();
		void	serverLoop();
};
