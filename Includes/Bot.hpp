#pragma once

#include "Server.hpp"

# include <cstdlib>
# include <ctime> 

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
		void Roll(int user, std::string nickname, std::vector<std::string> message);
		void joke(int sender, std::string nickname, std::vector<std::string> args);

		struct Die
		{
			int dieNumber;
			int dieFace;
			int modifier;
			bool advantage;
			bool disadvantage;
		};
		
};
