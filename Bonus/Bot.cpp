#include "../Includes/Bot.hpp"

#include <cerrno>

Bot::Bot(std::string name, Server *server): _name(name), _server(server), botFd(-1)
{
	srand(time(NULL));
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
		_server->getServChannels()[channelName].sendMessage(mess.str().c_str(), botFd);
	}
}

Bot::Die parseDie(std::vector<std::string> args)
{
	Bot::Die die;
	die.modifier = 0;
	die.advantage = false;
	die.disadvantage = false;
	if (args.size() > 3) 
	{
	    std::string diceArg = args[3];
	    size_t dPos = diceArg.find('d');
	    size_t modPos = diceArg.find_first_of("+-", dPos + 1);	
	    if (dPos != std::string::npos)
	    {
	        die.dieNumber = std::atoi(diceArg.substr(0, dPos).c_str());
	        die.dieFace = std::atoi(diceArg.substr(dPos + 1, modPos - dPos - 1).c_str());	
	        if (modPos != std::string::npos)
	            die.modifier = std::atoi(diceArg.substr(modPos).c_str());
	    }
		else
		{
			die.dieNumber = 1;
			die.dieFace = std::atoi(diceArg.c_str());
		}
	}
	if (args.size() > 4)
	{
		if (args[4] == "advantage") die.advantage = true;
		else if (args[4] == "disadvantage") die.disadvantage = true;
	}
	return die;
}

void Bot::Roll(int user, std::string nickname, std::vector<std::string> args)
{
	Die die = parseDie(args);
	int validDiceArray[] = {4, 6, 8, 10, 12, 20, 100};
	std::vector<int> validDice(validDiceArray, validDiceArray + 7);
	if (std::find(validDice.begin(), validDice.end(), die.dieFace) == validDice.end() || die.dieFace == 0)
	{
		std::ostringstream message;
    	message << ":" << _name << " PRIVMSG " << nickname << " :Die " << die.dieFace <<" :Invalid roll. Format: XdY+Z advantage/disadvantage.\r\n" << std::endl;
    	send(user, message.str().c_str(), message.str().size(), 0);
		return ;
	}
	std::vector<int> results;
	int total = 0, best = 0, worst = die.dieFace;
	
	for(int i = 0; i < die.dieNumber; ++i)
	{
		int r = (rand() % die.dieFace) + 1;
		results.push_back(r);
		total += r;
		if (r > best) best = r;
		if (r < worst) worst = r;
	}
	if (die.advantage) total = best + die.modifier;
	else if (die.disadvantage) total = worst + die.modifier;
	else total += die.modifier;
	std::ostringstream message;
	if (args[1] == _name)
	{
		message << ":" << _name << " PRIVMSG " << nickname << " :you rolled " << die.dieNumber << "d" << die.dieFace;
		if (die.modifier != 0)
			message << (die.modifier > 0 ? "+" : "") << die.modifier;
		message << " -> [";
		for (size_t i = 0; i < results.size(); i++)
		{
			message << results[i] << (i == results.size() - 1 ? "" : ", ");
		}
		message << "] Total: " << total  << "\r\n" << std::endl;
		send(user, message.str().c_str(), message.str().size(), 0);
	}
	else
	{
		message << ":" << _name << " PRIVMSG " << args[1] << " :" << nickname << " rolled  " << die.dieNumber << "d" << die.dieFace;
		if (die.modifier != 0)
			message << (die.modifier > 0 ? "+" : "") << die.modifier;
		message << " -> [";
		for (size_t i = 0; i < results.size(); i++)
		{
			message << results[i] << (i == results.size() - 1 ? "" : ", ");
		}
		message << "] Total: " << total  << "\r\n" << std::endl;
		_server->getServChannels()[args[1]].sendMessage(message.str().c_str(), botFd);
	}
}