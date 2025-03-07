/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 13:52:44 by kethouve          #+#    #+#             */
/*   Updated: 2025/03/07 16:38:25 by kethouve         ###   ########.fr       */
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

#include <stdio.h>

#include "User.hpp"
#include "Channels.hpp"
#include "Bot.hpp"

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
class Bot;

class Server
{
	enum commands
	{
		NICK,
		PRIVMSG,
		JOIN,
		KICK,
		TOPIC,
		INVITE,
		MODE,
		PART,
		QUIT,
		DISPLAYUSER,
		DISPLAYCHANNELS
	};

	private:
		std::string _serverPass;
		sockaddr_in _serverAddr;
		int			_serverPort;
		int			_serverfd;
		std::map<int, User> _user;
		std::map<std::string, Channels> _channels;
		std::map<int, std::string> clientBuffers;
		std::vector<pollfd> pollFds;
		Bot *_bot;
		Server();
	public:
		Server(int port, std::string pass);
		~Server();
		void	setServerSocket();
		void	userConnection(std::string message, int i);
		void	serverLoop();
		void	clearServ();

		void	join(std::vector<std::string> message, int user);
		void	kick(std::vector<std::string> message, int user);
		void	topic(std::vector<std::string> message, int user);
		void	invite(std::vector<std::string> message, int user);
		void	part(std::vector<std::string> message, int user);
		void	quit(std::vector<std::string> message, int user);
		void	nick(std::vector<std::string> message, size_t i);
		void	privmsg(std::vector<std::string> message, size_t i);
		void	user(std::vector<std::string> message, size_t i);
		void	pass(std::vector<std::string> message, size_t i);
		void	mode(std::vector<std::string> message, int user);
		
		std::vector<std::string>	parseMessage(std::string message);
		bool	isCommand(std::string message);
		Server::commands findCommand(std::string command);
		void	executeCommand(std::string message, int i);
		bool    nickExist(std::string nick);
		int		nicknameToFd(std::string nick);
		std::string fdToNickname(int fd);
		void	destroyUser(const int user);
		void	destroyChannel(std::string salon);
		void	displayUsers(std::string message, int sender);
		void	displayChannels(int sender);
		std::map<std::string, Channels> getServChannels();
		int		getServerFd();
};
