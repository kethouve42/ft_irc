#include "Bot.hpp"

Bot::Bot(std::string name, Server *server): _name(name), _server(server), botFd(-1)
{
	std::cout << MAGENTA << "[SERVER]" << BLUE << " Chatbot " << _name << " a rejoint le serveur." << RESET << std::endl;
}

Bot::~Bot()
{
	std::cout << MAGENTA << "[" << _name << "]" << RED << ": NOOOOOOOOOOOOOOOOOOOOOOOOOoooooooooo!!!!!!!" << RESET << std::endl;
}

void Bot::Welcome(int user, std::string nickname, std::string channelName)
{
	std::ostringstream message;
	if (channelName == "general")
    	message << ":" << _name << " PRIVMSG " << nickname << " :Bienvenue sur le serveur, tu as rejoit le canal " << channelName << ", " << nickname << "! 🎉\r\n";
	else
		message << ":" << _name << " PRIVMSG " << nickname << " :Tu as rejoit le canal " << channelName << ", " << nickname << "! 🎉\r\n";
    send(user, message.str().c_str(), message.str().size(), 0);
	if (channelName == "general")
	{
		std::string mess = ":" + _name + " PRIVMSG " + channelName + " :" + nickname + " a rejoint le serveur sur le canal: " + channelName + " 🎉\r\n";
		_server->getServChannels()[channelName].sendMessage(mess, botFd);
	}
	else
	{
		std::ostringstream mess;
		mess << ":" << _name << " PRIVMSG " << channelName << " :" << nickname << " a rejoint " << channelName << "! 🎉\r\n";
		std::cout << "message general: " << mess.str() << std::endl;
		_server->getServChannels()[channelName].sendMessage(mess.str().c_str(), botFd);
	}
}
	