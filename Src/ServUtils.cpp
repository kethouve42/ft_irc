/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 15:17:20 by kethouve          #+#    #+#             */
/*   Updated: 2025/03/07 22:13:29 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Server.hpp"

std::vector<std::string>	Server::parseMessage(std::string message)
{
	size_t pos = 0, last_pos = 0;
	std::vector<std::string> args;
	std::string temp;

	while ((pos = message.find(" ", last_pos)) != std::string::npos)
	{
		temp = message.substr(last_pos, pos - last_pos);
		temp.erase(temp.find_last_not_of(" \t\n\r") + 1);
		if (!temp.empty()) args.push_back(temp);
		last_pos = pos + 1;
	}

	temp = message.substr(last_pos);
	temp.erase(temp.find_last_not_of(" \t\n\r") + 1);
	if (!temp.empty()) args.push_back(temp);

	/*DEBUG*/
	/*std::cout << "Arguments:" << std::endl;
	for (size_t i = 0; i < args.size(); ++i)
	{
		std::cout << "args[" << i << "]: " << args[i] << std::endl;
	}*/
	return (args);

}

int Server::nicknameToFd(std::string nick)
{
    for (std::map<int, User>::iterator it = _user.begin(); it != _user.end(); ++it)
    {
        if (it->second.getUserNickName() == nick)
            return it->second.getUserFd();
    }
    return -1;
}

std::string Server::fdToNickname(int fd)
{
    for (std::map<int, User>::iterator it = _user.begin(); it != _user.end(); ++it)
    {
        if (it->second.getUserFd() == fd)
            return it->second.getUserNickName();
    }
    return std::string();
}

bool Server::nickExist(std::string nick)
{
	for (std::map<int, User>::iterator it = _user.begin(); it != _user.end(); ++it)
    {
        if (it->second.getUserNickName() == nick)
            return true;
    }
    return false;
}

void Server::displayChannels(int sender)
{
    std::string message = "list of channels: \n";
    for (std::map<std::string, Channels>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
        message += it->second.getChannelName() + "\n";
	}
	send(sender, message.c_str(), message.size(), 0);
}

void Server::displayUsers(std::string message, int sender)
{
	std::string salon = message.substr(12);
    salon.erase(salon.find_last_not_of(" \t\n\r") + 1);
    std::string userMessage = salon + "'s users :\n";
    for (std::map<int, User>::iterator it = _user.begin(); it != _user.end(); ++it)
	{
        if (_channels[salon].VerifUser(it->second.getUserFd()))
            userMessage += it->second.getUserNickName() + "\n";
	}
	send(sender, userMessage.c_str(), userMessage.size(), 0);
}

void Server::destroyUser(const int user)
{
    std::cout << MAGENTA << "[SERVER] " << GREEN << "Client déconnecté : " << user << " " << _user[user].getUserNickName() << RESET << std::endl;
	bool isClean = false;
	while (!isClean)
	{
		isClean = true;
		for (std::map<std::string, Channels>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			if (it->second.VerifUser(user))
			{
				isClean = false;
				it->second.deleteUser(user);
				destroyChannel(it->second.getChannelName());
				break;
			}
		}
	}
    if (_user.find(user) != _user.end()) {
        _user.erase(user);
    }
    for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it)
    {
        if (it->fd == user)
        {
            pollFds.erase(it);
            break;
        }
    }
    close(user);
}

void Server::destroyChannel(std::string salon)
{
	if (_channels[salon].getUsers().size() <= 0)
	{
		_channels.erase(salon);
		std::cout << MAGENTA << "[SERVER] " << RED << "Fermeture du salon " << salon << " faute d'utilisateur" << RESET << std::endl;
	}
}

bool Server::isCommand(std::string message)
{
	size_t pos = message.find(" ");
	std::string cmd = message.substr(0, pos);
	cmd.erase(cmd.find_last_not_of(" \t\n\r") + 1);
	if (cmd == "PASS" || cmd == "DISPLAYCHANNELS" || cmd == "USER" || cmd == "NICK" || cmd == "PRIVMSG" || cmd == "JOIN"
		|| cmd == "KICK" || cmd == "TOPIC" || cmd == "INVITE" || cmd == "MODE" || cmd == "PART"
		|| cmd == "QUIT" || cmd == "DISPLAYUSER")
	{
		return true;
	}
	return false;
}

Server::commands Server::findCommand(std::string command)
{
	if (command == "NICK") return NICK;
	if (command == "PRIVMSG") return PRIVMSG;
	if (command == "JOIN") return JOIN;
	if (command == "KICK") return KICK;
	if (command == "TOPIC") return TOPIC;
	if (command == "INVITE") return INVITE;
	if (command == "MODE") return MODE;
	if (command == "PART") return PART;
	if (command == "QUIT") return QUIT;
	if (command == "DISPLAYUSER") return DISPLAYUSER;
	if (command == "DISPLAYCHANNELS") return DISPLAYCHANNELS;
	throw std::invalid_argument("Unknown command: ");
}

void Server::executeCommand(std::string message, int i)
{
	std::vector<std::string> args = parseMessage(message);
	try
	{
		commands cmd = findCommand(args[0]);
		switch (cmd)
		{
			case NICK:
				nick(args, i);
				break;
			case PRIVMSG:
				privmsg(args, i);
				break;
			case JOIN:
				join(args, pollFds[i].fd);
				break;
			case KICK:
				kick(args, pollFds[i].fd);
				break;
			case TOPIC:
				topic(args, pollFds[i].fd);
				break;
			case INVITE:
				invite(args, pollFds[i].fd);
				break;
			case MODE:
				mode(args, pollFds[i].fd);
				break;
			case PART:
				part(args, pollFds[i].fd);
				break;
			case QUIT:
				quit(args, pollFds[i].fd);
				break;
			case DISPLAYUSER:
				displayUsers(message, pollFds[i].fd);
				break;
			case DISPLAYCHANNELS:
				displayChannels(pollFds[i].fd);
				break;
			
			default:
				break;
		}
		return ;
	}
	catch(const std::invalid_argument &e)
	{
		std::string errorMsg = "NOTICE : Unknown command: " + args[0] + "\r\n";
		send(pollFds[i].fd, errorMsg.c_str(), errorMsg.length(), 0);

	}
}
