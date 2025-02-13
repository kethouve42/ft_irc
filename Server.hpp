/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 13:52:44 by kethouve          #+#    #+#             */
/*   Updated: 2025/02/13 15:50:12 by kethouve         ###   ########.fr       */
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
#include <algorithm> 

#include "User.hpp"
#include "Channels.hpp"

#define BUFFER_SIZE 1024

const std::string GREY =	"\033[0;30m";
const std::string GREEN =   "\033[0;32m";
const std::string YELLOW =  "\033[0;33m";
const std::string BLUE =    "\033[0;34m";
const std::string MAGENTA = "\033[0;35m";
const std::string CYAN =    "\033[0;36m";
const std::string WHITE =	"\033[0;37m";
const std::string RED = 	"\033[0;31m";
const std::string UNDERLINE =  "\033[4m";
const std::string BOLD =    "\033[1m";
const std::string RESET =	"\033[0m";

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
		std::map<int, std::string> clientBuffers;
		std::vector<pollfd> pollFds;
		Server();
	public:
		Server(int port, std::string pass);
		~Server();
		void	setServerSocket();
		void	serverLoop();
		void	join(std::string message, int user);
		void	kick(std::string message, int user);
		void	topic(std::string message, int user);
		void	invite(std::string message, int user);
		void	part(std::string message, int user);
		void	quit(std::string message, int user);
		void	mode(std::string message, int user);
		void	destroyUser(const int user);
		void	destroyChannel(std::string salon);
		bool    nickExist(std::string nick);
		void	nick(std::string message, size_t i);
		void	privmsg(std::string message, size_t i);
		void	user(std::string message, size_t i);
		void	pass(std::string message, size_t i);
		void	clearServ();
		int		nicknameToFd(std::string nick);
		std::string fdToNickname(int fd);
		void	displayUsers(std::string salon, int sender);
		void	displayChannels(int sender);
};
