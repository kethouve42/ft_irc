#pragma once

#include "Server.hpp"

class Server;

class Bot
{
	private:
		std::string _name;
		Server *_server;
		Bot();
		
	public:
		int botFd;
		Bot(std::string name, Server *server);
		~Bot();
		void Welcome(int user, std::string name, std::string channelName);
};
