/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 15:51:29 by kethouve          #+#    #+#             */
/*   Updated: 2025/03/07 22:12:29 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Server.hpp"

void Server::nick(std::vector<std::string> message, size_t i)
{
	if (message.size() < 2)
    {
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " NICK :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << "Message of " << _user[pollFds[i].fd].getUserNickName() << " has not enough argument." << RESET << std::endl;
        return;
    }
	if (!nickExist(message[1]))
	{
        if (message[1][0] == ':')
			message[1].erase(0, 1);
		std::string nickChanged = ":" + _user[pollFds[i].fd].getUserNickName() + " NICK " + message[1] + "\r\n";
		send(pollFds[i].fd, nickChanged.c_str(), nickChanged.size(), 0);
		if (_user[pollFds[i].fd].is_user == true)
			std::cout << MAGENTA << "[SERVER] " << GREEN << _user[pollFds[i].fd].getUserNickName() << " successfuly change is nickname for " << message[1] << std::endl;
		else
			std::cout << MAGENTA << "[SERVER] " << GREEN << _user[pollFds[i].fd].getUserNickName() << " nickname is " << message[1] << std::endl;
		_user[pollFds[i].fd].setUserNickName(message[1]);
		_user[pollFds[i].fd].nick = true;
	}
	else
	{
		std::string errMessage = "NOTICE : " + message[1] + " existe deja trouvé un autre NickName!\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << message[1] << " existe deja. Echec du changement de nickname!" << RESET << std::endl;
		return;
	}
}

void Server::privmsg(std::vector<std::string> message, size_t i)
{
	std::string salon, msgContent;

	if (message.size() > 2)
	{
		salon = message[1];
		for (size_t i = 2; i < message.size(); ++i)
		{
			if (!msgContent.empty()) msgContent += " ";
			msgContent += message[i];
		}
	}
	if (msgContent.empty())
    {
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " PRIVMSG :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << "Message of " << _user[pollFds[i].fd].getUserNickName() << " has not enough argument." << RESET << std::endl;
        return;
    }
    if (msgContent[0] == ':')
        msgContent.erase(0, 1);
    if (salon[0] == '#')
    {
        if (_channels[salon].VerifUser(pollFds[i].fd) == true)
        {
            std::stringstream messageStream;
            messageStream << ":" << _user[pollFds[i].fd].getUserNickName() << " PRIVMSG " 
                        << salon << " :" << msgContent << "\r\n";
            std::string messageToSend = messageStream.str();
            _channels[salon].sendMessage(messageToSend, pollFds[i].fd);
			std::cout << CYAN << "[" << _user[pollFds[i].fd].getUserNickName() << "] in " << salon << RESET << BOLD << ": " << msgContent << std::endl;
        }
        else
            std::cout << MAGENTA << "[SERVER] " << RED << _user[pollFds[i].fd].getUserNickName() << " is not a user of :" << salon << RESET << std::endl;
    }
    else
    {
        std::map<int, User>::iterator it;
        for (it = _user.begin(); it != _user.end(); ++it)
        {
            if (it->second.getUserNickName() == salon)
            {
                std::stringstream messageStream;
                messageStream << ":" << _user[pollFds[i].fd].getUserNickName() 
                    << " PRIVMSG " << salon 
                    << " :" << msgContent << "\r\n";
                std::string messageToSend = messageStream.str();
                send(it->first, messageToSend.c_str(), messageToSend.size(), 0);
				std::cout << CYAN << "[" << _user[pollFds[i].fd].getUserNickName() << "] to " << salon << RESET << BOLD << ": " << msgContent << std::endl;
                return;
            }
        }
        // Si le destinataire n'existe pas, envoyer un message d'erreur
        std::string errMessage = ":server 401 " + _user[pollFds[i].fd].getUserNickName() + 
            " " + salon + " :No such nick/channel\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << salon << " not found. Send fail!" << RESET << std::endl;
    }
}

/*Set du user*/
void Server::user(std::vector<std::string> message, size_t i)
{
	std::string username;
    std::string hostname;
    std::string servername;
    std::string realname;

	if(message.size() > 1) username = message[1];
	if(message.size() > 2) hostname = message[2];
	if(message.size() > 3) servername = message[3];
	if(message.size() > 4) realname = message[4];
    if (realname.size() != 0)
    {
        if (realname[0] == ':')
            realname.erase(0, 1);
        _user[pollFds[i].fd].setUserRealname(realname);
        _user[pollFds[i].fd].setUserName(username);
        _user[pollFds[i].fd].user = true;
		std::cout << MAGENTA << "[SERVER] " << GREEN << "Real name set" << RESET << std::endl;
		std::string userReply = ":server 001 " + message[1] + " :Welcome to the IRC Server!\r\n";
    	send(pollFds[i].fd, userReply.c_str(), userReply.size(), 0);
    }
    else
    {
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " USER :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << "Message of " << _user[pollFds[i].fd].getUserNickName() << " has not enough argument." << RESET << std::endl;
        return;
    }
}

void Server::pass(std::vector<std::string> message, size_t i)
{
    if (message.size() < 2)
    {
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " PASS :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << "Message of " << _user[pollFds[i].fd].getUserNickName() << " has not enough argument." << RESET << std::endl;
        return;
    }
    if (message[1][0] == ':')
    {
        if (":" + _serverPass == message[1])
            _user[pollFds[i].fd].pass = true;
        else
            destroyUser(pollFds[i].fd);
    }
    else
    {
        if (_serverPass == message[1])
            _user[pollFds[i].fd].pass = true;
        else
            destroyUser(pollFds[i].fd);
    }
}

/* Ajoute le user au salon */
void Server::join(std::vector<std::string> message, int user)
{
    std::string joinSalon, passWord;
    std::string nickname = _user[user].getUserNickName();

	if (message.size() > 1) joinSalon = message[1];
	if (message.size() > 2) passWord = message[2];
    if (passWord[0] == ':')
        passWord.erase(0, 1);

    // Salon vide
    if (joinSalon.empty() || joinSalon[0] != '#')
    {
        std::string errMessage = ":server 461 " + nickname + " JOIN :Not enough parameters\r\n";
        send(user, errMessage.c_str(), errMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << "Message of " << _user[pollFds[user].fd].getUserNickName() << " has not enough argument." << RESET << std::endl;
        return;
    }

    // Création de salon
    if (_channels.find(joinSalon) == _channels.end())
    {
        std::cout << MAGENTA << "[SERVER] " << GREEN << "Salon " << joinSalon << " créé" << RESET << std::endl;
        Channels nouveauSalon(joinSalon, user);
        _channels[joinSalon] = nouveauSalon;

        std::string createMessage = ":" + nickname + " JOIN :" + joinSalon + "\r\n";
        send(user, createMessage.c_str(), createMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << GREEN << _user[user].getUserNickName() << " a rejoint le salon " << joinSalon << RESET << std::endl;
        return;
    }

    if ((int)_channels[joinSalon].getUsers().size() >= _channels[joinSalon].getUserLimit())
    {
        std::string fullMessage = ":server 471 " + nickname + " " + joinSalon + " :Channel is full\r\n";
        send(user, fullMessage.c_str(), fullMessage.size(), 0);
		std::cout << "[SERVER] " << RED << _user[user].getUserNickName() << " ne peut pas rejoindre " << joinSalon << " puisqu'il est plein" << RESET << std::endl;
        return;
    }

    if (_channels[joinSalon].getPassword() != "")
    {
        if (_channels[joinSalon].getPassword() != passWord)
        {
            std::string fullMessage = ":server 471 " + nickname + " " + joinSalon + " :Wrong password\r\n";
            send(user, fullMessage.c_str(), fullMessage.size(), 0);
			std::cout << "[SERVER] " << RED << _user[user].getUserNickName() << " ne peut pas rejoindre " << joinSalon << " mauvais password" << RESET << std::endl;
            return ;
        }
    }

    // Ajout de l'utilisateur au salon
    if (_channels[joinSalon].addUser(user, fdToNickname(user)) == 0)
    {

        std::string joinMessage = ":" + nickname + " JOIN :" + joinSalon + "\r\n";
        send(user, joinMessage.c_str(), joinMessage.size(), 0);
        _channels[joinSalon].sendMessage(joinMessage, user);
		std::cout << MAGENTA << "[SERVER] " << GREEN << _user[user].getUserNickName() << " a rejoit le salon " << joinSalon << RESET << std::endl;

        // Construction de la liste des utilisateurs
        std::string namesMessage = ":server 353 " + nickname + " = " + joinSalon + " :";
        std::vector<int> users = _channels[joinSalon].getUsers();
        std::vector<int>::iterator it;
        for (it = users.begin(); it != users.end(); ++it)
        {
            namesMessage += _user[*it].getUserNickName() + " ";
        }
        namesMessage += "\r\n";

        // Fin de la liste des utilisateurs
        std::string endNamesMessage = ":server 366 " + nickname + " " + joinSalon + " :End of /NAMES list\r\n";

        send(user, namesMessage.c_str(), namesMessage.size(), 0);
        send(user, endNamesMessage.c_str(), endNamesMessage.size(), 0);
		_channels[joinSalon].sendMessage(namesMessage, user);

        // Envoi du topic s'il existe
        if (_channels[joinSalon].getTopic() != "")
        {
            std::string topicMessage = ":server 332 " + nickname + " " + joinSalon + " :" + _channels[joinSalon].getTopic() + "\r\n";
            send(user, topicMessage.c_str(), topicMessage.size(), 0);
        }
        else
        {
            std::string noTopicMessage = ":server 331 " + nickname + " " + joinSalon + " :No topic is set\r\n";
            send(user, noTopicMessage.c_str(), noTopicMessage.size(), 0);
        }
    }
}

/* Retire un user du salon */
void Server::kick(std::vector<std::string> message, int user)
{
	std::string userKickName, salon, messageKick;

	if (message.size() > 1) salon = message[1];
	if (message.size() > 2) userKickName = message[2];
	if (message.size() > 3)
	{
		for (size_t i = 3; i < message.size(); ++i)
		{
			if (!messageKick.empty()) messageKick += " ";
			messageKick += message[i];
		}
	}
	std::istringstream ss(userKickName);
	int userKick;
    ss >> userKick;
    if (ss.fail())
	{
		bool userFound = false;
        for (std::map<int, User>::iterator it = _user.begin(); it != _user.end(); ++it)
        {
            if (it->second.getUserNickName() == userKickName)
            {
                userKick = it->first;
                userFound = true;
                break;
            }
        }
        if (!userFound)
        {
            std::cerr << MAGENTA << "[SERVER] " << RED << "Utilisateur avec nickname ou ID " << userKickName << " introuvable." << RESET << std::endl;
            return;
        }
	}
	if (_channels[salon].VerifAdmin(user))
	{
		std::string kickMessage = "NOTICE : " + fdToNickname(userKick) + " a été kick du salon '" + salon + "'\r\n";
    	_channels[salon].sendMessage(kickMessage, userKick);
        if (messageKick != " ")
            kickMessage += "NOTICE : Message de kick: " + messageKick + "\r\n";
        send(userKick, kickMessage.c_str(), kickMessage.size(), 0);
		std::string userMessage = ":" + _user[userKick].getUserNickName() + " PART " + salon + " : Goodbye!\r\n";
		send(userKick, userMessage.c_str(), userMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << GREEN << fdToNickname(userKick) << " a été kick du salon " << salon << RESET << std::endl;
		_channels[salon].deleteUser(userKick);
		destroyChannel(salon);
	}
    else if (!_channels[salon].VerifAdmin(user))
    {
        std::string notAdmin = "NOTICE : No permission for kick in " + salon + "\r\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << fdToNickname(user) << " n'est pas admin du salon " << salon << RESET << std::endl;
    }
    else
    {
        std::string notAdmin = "NOTICE : Error occurred in the request\r\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << fdToNickname(user) << " ERREUR" << RESET << std::endl;
    }
}

/* Retire le user du salon */
void Server::part(std::vector<std::string> message, int user)
{
    std::string messagePart, partSalon;

	if (message.size() > 1) partSalon = message[1];
	if (message.size() > 2)
	{
		for (size_t i = 2; i < message.size(); ++i)
		{
			if (!messagePart.empty()) messagePart += " ";
			messagePart += message[i];
		}
	}
    if (partSalon.empty())
    {
        std::string errMessage = "NOTICE : Pas assez de paramètres.\n";
        send(user, errMessage.c_str(), errMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << "Message of " << _user[user].getUserNickName() << " has not enough argument." << RESET << std::endl;
        return ;
    }

    if (_channels.find(partSalon) == _channels.end())
    {
        std::cout << "Le salon " << partSalon << " n'existe pas" << std::endl;
		std::string notExisting = "NOTICE : This channel doesn't exist\r\n";
        send(user, notExisting.c_str(), notExisting.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << "Salon " << partSalon << " n'existe pas" << RESET << std::endl;
        return;
    }

    std::string partMessage = "NOTICE : " + fdToNickname(user) + " a quitte le salon '" + partSalon + "'\r\n";
    if (messagePart != " ")
		partMessage += "NOTICE : Message de Part: " + messagePart + "\r\n";
    _channels[partSalon].sendMessage(partMessage, user);
    send(user, partMessage.c_str(), partMessage.size(), 0);
	std::string userMessage = ":" + _user[user].getUserNickName() + " PART " + partSalon + " :Goodbye!\r\n";
	send(user, userMessage.c_str(), userMessage.size(), 0);
    _channels[partSalon].deleteUser(user);
	std::cout << MAGENTA << "[SERVER] " << GREEN << _user[user].getUserNickName() << " a quitté le salon " << partSalon << RESET << std::endl;
	destroyChannel(partSalon);
    return;
}

void Server::quit(std::vector<std::string> message, int user)
{
	std::string quitMessage;
	if (message.size() > 1)
	{
		for (size_t i = 1; i < message.size(); ++i)
		{
			if (!quitMessage.empty()) quitMessage += " ";
			quitMessage += message[i];
		}
	}
    if (quitMessage[0] == ':')
        quitMessage.erase(0, 1);
    if (!quitMessage.empty())
    {
        std::string quitServeur = "QUIT :" + fdToNickname(user) + " quitte le serveur : " + quitMessage + "\n";
        _channels["#general"].sendMessage(quitServeur, user);
    }
	else
	{
		std::string quitServeur = "QUIT :" + fdToNickname(user) + " quitte le serveur" + "\n";
        _channels["#general"].sendMessage(quitServeur, user);
	}
    destroyUser(user);
}

void Server::invite(std::vector<std::string> message, int user)
{
    std::string userInviteName, salon;

	if (message.size() > 1) salon = message[1];
	if (message.size() > 2) userInviteName = message[2];
    if (_channels.find(salon) == _channels.end())
    {
        std::cout << MAGENTA << "[SERVER] " << RED << "Le salon " << salon << " n'existe pas" << RESET << std::endl;
		std::string notExisting = "NOTICE : This channel doesn't exist\r\n";
        send(user, notExisting.c_str(), notExisting.size(), 0);
        return;
    }

    userInviteName.erase(userInviteName.find_last_not_of(" \t\n\r") + 1);
	std::istringstream ss(userInviteName);
	int userInvite;
    ss >> userInvite;
    if (ss.fail())
	{
        userInvite = nicknameToFd(userInviteName);
        if (userInvite == -1)
        {
            std::cerr << MAGENTA << "[SERVER] " << RED << "Utilisateur avec nickname ou ID " << userInviteName << " introuvable." << RESET << std::endl;
			std::string notUser = "NOTICE : user " + userInviteName + " not found \r\n";
        	send(user, notUser.c_str(), notUser.size(), 0);
            return;
        }
	}

    if (_channels[salon].VerifUser(user))
        _channels[salon].addInvited(userInvite, fdToNickname(user), fdToNickname(userInvite));
    else
	{
		std::string notMember = "NOTICE : You're not a member of this channel, you can't invit\r\n";
        send(user, notMember.c_str(), notMember.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << _user[user].getUserNickName() << " n'est pas membre du salon " << salon << RESET << std::endl;
	}

}

/* Afficher ou changer le sujet d'un salon */
void Server::topic(std::vector<std::string> message, int user)
{
	std::string salon;
	std::string newTopic;

	if (message.size() > 1)
	{
		salon = message[1];
		if (message.size() > 2)
		{
			for (size_t i = 2; i < message.size(); ++i)
			{
				if (!newTopic.empty()) newTopic += " ";
				newTopic += message[i];
			}
		}
	}
    if (newTopic[0] == ':')
        newTopic.erase(0, 1);
    if (_channels.find(salon) == _channels.end())
    {
        std::cout << MAGENTA << "[SERVER] " << RED << "impossible de changer le topic. Le salon " << salon << " n'existe pas" << RESET << std::endl;
        return;
    }

    if (_channels[salon].getInvitMode() && !_channels[salon].VerifUser(user))
    {
        std::string notMember = "NOTICE : " + _user[user].getUserNickName() + " Topic available only to members of " + salon + "\r\n";
        send(user, notMember.c_str(), notMember.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << "impossible de changer le topic." << fdToNickname(user)  << " n'est pa membre du salon " << salon << RESET << std::endl;
        return;
    }

    if (newTopic.empty() && !_channels[salon].getTopic().empty())
    {
        std::string topicMessage = "NOTICE : TOPIC of " + salon + " : " + _channels[salon].getTopic() + "\r\n";
        send(user, topicMessage.c_str(), topicMessage.size(), 0);
    }
    else if (newTopic.empty() && _channels[salon].getTopic().empty())
    {
        std::string noTopic = "NOTICE : " + salon + " has no topic yet\r\n";
        send(user, noTopic.c_str(), noTopic.size(), 0);
    }
    else if (_channels[salon].getRestrictedTopic() && !_channels[salon].VerifAdmin(user))
    {
        std::string notAdmin = "NOTICE : No permission to change topic on " + salon + "\r\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << fdToNickname(user) << " n'est pas admin du salon " << salon << RESET << std::endl;
    }
    else
    {
        _channels[salon].setTopic(newTopic, user);

        std::string newTopicMessage = "NOTICE : New topic on " + salon + " : " + newTopic + "\r\n";
        _channels[salon].sendMessage(newTopicMessage, user);
    }

}

/* Change les parametres du salon */
void Server::mode(std::vector<std::string> message, int user)
{
	std::string commande;
    std::string salon;
    std::string param;

	if (message.size() > 1)
	{
		salon = message[1];
		if (message.size() > 2)
		{
			commande = message[2];
			for (size_t i = 3; i < message.size(); ++i)
			{
				if (!param.empty()) param += " ";
				param += message[i];
			}
		}
	}
    if (_channels.find(salon) == _channels.end())
    {
        std::cout << MAGENTA << "[SERVER] " << RED << "Le salon " << salon << " n'existe pas" << RESET << std::endl;
		std::string notExisting = "NOTICE : This channel doesn't exist\r\n";
        send(user, notExisting.c_str(), notExisting.size(), 0);
        return;
    }
	else if (commande.empty() || commande == "+b")
		return; 
    else if (!_channels[salon].VerifAdmin(user))
    {
        std::string notAdmin = "NOTICE : " + salon + " only admins can use MODE (so you're not)\r\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << RED << fdToNickname(user) << " n'est pas admin du salon " << salon << RESET << std::endl;
        return;
    }

    if (commande == "+i" || commande == "-i")
    {
        _channels[salon].setInvitationMode(commande, user);
    }
    else if (commande == "+t" || commande == "-t")
    {
        _channels[salon].setRestrictedTopic(commande, user, _user[user].getUserNickName());
    }
    else if (commande == "+k" && !param.empty())
    {
        _channels[salon].setChannelPass(param, user);
    }
    else if (commande == "-k")
    {
        _channels[salon].setChannelPass("", user);
    }
    else if (commande == "+o" || commande == "-o")
    {
        if(_channels[salon].VerifUser(nicknameToFd(param)) > 0)
        {
            if(commande == "+o")
                _channels[salon].addAdmin(nicknameToFd(param), fdToNickname(user), param);
            else if (param != fdToNickname(user))
                _channels[salon].removeAdmin(nicknameToFd(param));
        }
        else
		{
			std::string noParam = "NOTICE : Not enough parameters: <MODE> <channel> <parameter> <username>\r\n";
        	send(user, noParam.c_str(), noParam.size(), 0);
			std::cout << MAGENTA << "[SERVER] " << RED << "Message of " << _user[pollFds[user].fd].getUserNickName() << " has not enough argument." << RESET << std::endl;
		}
    }
    else if (commande == "+l" && !param.empty())
    {
        std::stringstream ss(param);
        int value;
        ss >> value;
        if (ss.fail())
        {
            std::cerr << "Error value." << std::endl;
        }
        else if (value > 0)
            _channels[salon].setUserLimit(value, user);
    }
    else if (commande == "-l")
    {
        _channels[salon].setUserLimit(INT_MAX, user);
    }
}
