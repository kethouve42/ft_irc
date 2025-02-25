/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 15:51:29 by kethouve          #+#    #+#             */
/*   Updated: 2025/02/25 16:48:31 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::nick(std::vector<std::string> message, size_t i)
{
	/*std::string userNickName = message.substr(5);
    userNickName.erase(userNickName.find_last_not_of(" \t\n\r") + 1);*/
	if (message.size() < 2)
    {
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " NICK :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
        return;
    }
	if (!nickExist(message[1]))
	{
        if (message[1][0] == ':')
			message[1].erase(0, 1);
		std::string nickChanged = ":" + _user[pollFds[i].fd].getUserNickName() + " NICK " + message[1] + "\r\n";
		send(pollFds[i].fd, nickChanged.c_str(), nickChanged.size(), 0);
		_user[pollFds[i].fd].setUserNickName(message[1]);
		_user[pollFds[i].fd].nick = true;
	}
	else
	{
		std::string errMessage = "NOTICE : " + message[1] + " existe deja trouvé un autre NickName!\r\n"; //trouver le code
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
		std::cout << message[1] << " existe deja trouvé un autre NickName!" << std::endl;
		return;
	}
}

void Server::privmsg(std::vector<std::string> message, size_t i)
{
	std::cout << "PRIVMSG\n";
	std::string salon, msgContent;
	/*std::string temp;
	std::string salon;
	std::string msgContent;*/
   /*size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        temp = message.substr(8, pos - 8);
        size_t pos = temp.find(" ");
        salon = temp.substr(0, pos);
        msgContent = temp.substr(pos + 1);
    }*/

	salon = message[1];
	if (message.size() > 2)
	{
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
        return;
    }
    if (msgContent[0] == ':')
        msgContent.erase(0, 1);
    std::cout << "msg :" << msgContent << "salon :" << salon << "\n";
    if (salon[0] == '#')
    {
        if (_channels[salon].VerifUser(pollFds[i].fd) == true)
        {
            std::stringstream messageStream;
            messageStream << ":" << _user[pollFds[i].fd].getUserNickName() << " PRIVMSG " 
                        << salon << " :" << msgContent << "\r\n";
            std::string messageToSend = messageStream.str();
            _channels[salon].sendMessage(messageToSend, pollFds[i].fd);
        }
        else
            std::cout << "no user of :" << salon << "\n";//faire le message si le user envois un message dans un channel mais est pas a l'interieur
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
                return;
            }
        }
        // Si le destinataire n'existe pas, envoyer un message d'erreur
        std::string errMessage = ":server 401 " + _user[pollFds[i].fd].getUserNickName() + 
            " " + salon + " :No such nick/channel\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
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

    /*size_t start = 5;
    size_t pos = message.find(" ", start);
    if (pos != std::string::npos)
    {
        username = message.substr(start, pos - start);
        start = pos + 1;

        pos = message.find(" ", start);
        if (pos != std::string::npos)
        {
            hostname = message.substr(start, pos - start);
            start = pos + 1;

            pos = message.find(" ", start);
            if (pos != std::string::npos)
            {
                servername = message.substr(start, pos - start);
                start = pos + 1;
                realname = message.substr(start);
            }
        }
    }*/
    if (realname.size() != 0)
    {
		std::cout << "\ni: " << pollFds[i].fd << "\n" << std::endl;
        if (realname[0] == ':')
            realname.erase(0, 1);
        _user[pollFds[i].fd].setUserRealname(realname);
        _user[pollFds[i].fd].setUserName(username);
        _user[pollFds[i].fd].user = true;
    }
    else /* problemme si la commande user est reutilisé correctement ou non */
    {
		std::cout << "\ni: " << pollFds[i].fd << "\n" << std::endl; // SOUCIS avec pollFds (invalid read) attribué sur server.cpp ligne 115
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " USER :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
        return;
    }
}

void Server::pass(std::vector<std::string> message, size_t i)
{
	/*std::string userPass = message.substr(5);
    userPass.erase(userPass.find_last_not_of(" \t\n\r") + 1);*/
    if (message.size() < 2)
    {
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " PASS :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
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
    /*size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        std::string temp = message.substr(5);
        size_t pos = temp.find(" ");
        joinSalon = temp.substr(0, pos);
        passWord = temp.substr(pos + 1);
    }
    joinSalon.erase(joinSalon.find_last_not_of(" \t\n\r") + 1);
    passWord.erase(passWord.find_last_not_of(" \t\n\r") + 1);*/
    if (passWord[0] == ':')
        passWord.erase(0, 1);
    std::cout << "JoinSalon : '" << joinSalon << "'\nPassword : '" << passWord << "'\n";

    // Salon vide
    if (joinSalon.empty() || joinSalon[0] != '#')
    {
        std::string errMessage = ":server 461 " + nickname + " JOIN :Not enough parameters\r\n";
        send(user, errMessage.c_str(), errMessage.size(), 0);
        return;
    }

    // Création de salon
    if (_channels.find(joinSalon) == _channels.end())
    {
        std::cout << "Salon " << joinSalon << " créé" << std::endl;
        Channels nouveauSalon(joinSalon, user);
        _channels[joinSalon] = nouveauSalon;

        std::string createMessage = ":" + nickname + " JOIN :" + joinSalon + "\r\n";
        send(user, createMessage.c_str(), createMessage.size(), 0);
        return;
    }

    if ((int)_channels[joinSalon].getUsers().size() >= _channels[joinSalon].getUserLimit())
    {
        std::string fullMessage = ":server 471 " + nickname + " " + joinSalon + " :Channel is full\r\n";
        send(user, fullMessage.c_str(), fullMessage.size(), 0);
        return;
    }

    if (_channels[joinSalon].getPassword() != "")
    {
        if (_channels[joinSalon].getPassword() != passWord)
        {
            std::string fullMessage = ":server 471 " + nickname + " " + joinSalon + " :Wrong password\r\n";
            send(user, fullMessage.c_str(), fullMessage.size(), 0);
            return ;
        }
    }

    // Ajout de l'utilisateur au salon
    if (_channels[joinSalon].addUser(user) == 0)
    {

        std::string joinMessage = ":" + nickname + " JOIN :" + joinSalon + "\r\n";
        send(user, joinMessage.c_str(), joinMessage.size(), 0);
        _channels[joinSalon].sendMessage(joinMessage, user);

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
        std::string endNamesMessage = ":server 366 " + nickname + " " + joinSalon + " :End of /NAMES list\r\n"; // <== WHY ???

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

	/*size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        std::string temp = message.substr(5);
        size_t pos = temp.find(" ");
        salon = temp.substr(0, pos);
        userKickName = temp.substr(pos + 1);
    }
    if (userKickName.find(":") != std::string::npos)
    {
        size_t pos = userKickName.find(" ");
        if (pos != std::string::npos)
        {
            std::string temp = userKickName.substr(0, pos);
            std::string remaining = userKickName.substr(pos + 1);
            messageKick = remaining;
            userKickName = temp;
        }
        if (messageKick[0] == ':')  
            messageKick.erase(0, 1);
    }
    userKickName.erase(userKickName.find_last_not_of(" \t\n\r") + 1);
    messageKick.erase(messageKick.find_last_not_of(" \t\n\r") + 1);*/
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
            std::cerr << "Utilisateur avec nickname ou ID " << userKickName << " introuvable.\n";
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
		//userMessage = "NOTICE : You've been kick of " + salon;
		//send(userKick, userMessage.c_str(), userMessage.size(), 0);
		_channels[salon].deleteUser(userKick);
		destroyChannel(salon);
	}
    else if (!_channels[salon].VerifAdmin(user))
    {
        std::string notAdmin = "NOTICE : No permission for kick in " + salon + "\r\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
    }
    else
    {
        std::string notAdmin = "NOTICE : Error occurred in the request\r\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
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
    /*std::string partSalon = message.substr(5);
    if (partSalon.find(":") != std::string::npos)
    {
        size_t pos = partSalon.find(" ");
        if (pos != std::string::npos)
        {
            std::string temp = partSalon.substr(0, pos);
            std::string remaining = partSalon.substr(pos + 1);
            messagePart = remaining;
            partSalon = temp;
        }
        if (messagePart[0] == ':')  
            messagePart.erase(0, 1);
        messagePart.erase(messagePart.find_last_not_of(" \t\n\r") + 1);
    }
    partSalon.erase(partSalon.find_last_not_of(" \t\n\r") + 1);*/
    if (partSalon.empty())
    {
        std::string errMessage = "NOTICE : Pas assez de paramètres.\n";
        send(user, errMessage.c_str(), errMessage.size(), 0);
        return ;
    }

    if (_channels.find(partSalon) == _channels.end())
    {
        std::cout << "Le salon " << partSalon << " n'existe pas" << std::endl;
		std::string notExisting = "NOTICE : This channel doesn't exist\r\n";
        send(user, notExisting.c_str(), notExisting.size(), 0);
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
    /*std::string quitMessage = message.substr(5);
    quitMessage.erase(quitMessage.find_last_not_of(" \t\n\r") + 1);*/
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
	/*size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        std::string temp = message.substr(7);
        size_t pos = temp.find(" ");
        salon = temp.substr(0, pos);
        userInviteName = temp.substr(pos + 1);
    }*/

    if (_channels.find(salon) == _channels.end())
    {
        std::cout << "Le salon " << salon << " n'existe pas" << std::endl;
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
            std::cerr << "Utilisateur avec nickname ou ID " << userInviteName << " introuvable.\n";
			std::string notUser = "NOTICE : user " + userInviteName + " not found \r\n";
        	send(user, notUser.c_str(), notUser.size(), 0);
            return;
        }
	}

    if (_channels[salon].VerifUser(user))
        _channels[salon].addInvited(userInvite);
    else
	{
		std::string notMember = "NOTICE : You're not a member of this channel, you can't invit\r\n";
        send(user, notMember.c_str(), notMember.size(), 0);
	}

}

/* Afficher ou changer le sujet d'un salon */
void Server::topic(std::vector<std::string> message, int user)
{
	std::string salon = message[1];
	std::string newTopic;

	if (message.size() > 2)
	{
		for (size_t i = 2; i < message.size(); ++i)
		{
			if (!newTopic.empty()) newTopic += " ";
			newTopic += message[i];
		}
	}

    /*message.erase(0, message.find_first_not_of(" \t\n\r"));
    message.erase(message.find_last_not_of(" \t\n\r") + 1);

    size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        std::string temp = message.substr(pos + 1);

        temp.erase(0, temp.find_first_not_of(" \t\n\r"));
        temp.erase(temp.find_last_not_of(" \t\n\r") + 1);

        size_t spacePos = temp.find(" ");
        salon = temp.substr(0, spacePos);

        if (spacePos != std::string::npos)
        {
            newTopic = temp.substr(spacePos + 1);
            newTopic.erase(newTopic.find_last_not_of(" \t\n\r") + 1);
        }
    }*/
    if (newTopic[0] == ':')
        newTopic.erase(0, 1);
    if (_channels.find(salon) == _channels.end())
    {
        std::cout << "Le salon " << salon << " n'existe pas" << std::endl;
        return;
    }

    if (_channels[salon].getInvitMode() && !_channels[salon].VerifUser(user))
    {
        std::string notMember = "NOTICE : " + _user[user].getUserNickName() + " Topic available only to members of " + salon + "\r\n";
        send(user, notMember.c_str(), notMember.size(), 0);
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
	std::cout << "MODE ARG:\nsalon: " << salon << " commande: " << commande << " param: " << param << std::endl;

    /*message.erase(0, message.find_first_not_of(" \t\n\r"));
    message.erase(message.find_last_not_of(" \t\n\r") + 1);
    
    size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        std::string temp = message.substr(pos + 1);
        temp.erase(0, temp.find_first_not_of(" \t\n\r"));
        temp.erase(temp.find_last_not_of(" \t\n\r") + 1);

        size_t spacePos = temp.find(" ");
        salon = temp.substr(0, spacePos);
    
        if (spacePos != std::string::npos)
        {
            std::string rest = temp.substr(spacePos + 1);
            rest.erase(0, rest.find_first_not_of(" \t\n\r"));
            rest.erase(rest.find_last_not_of(" \t\n\r") + 1);
    
            size_t secondSpacePos = rest.find(" ");
            commande = rest.substr(0, secondSpacePos);
    
            if (secondSpacePos != std::string::npos)
            {
                param = rest.substr(secondSpacePos + 1);
                param.erase(0, param.find_first_not_of(" \t\n\r"));
                param.erase(param.find_last_not_of(" \t\n\r") + 1);
            }
        }
    }*/
    // Debug : Afficher les parties extraites
    /*
    std::cout << "Salon : '" << salon << "'\n";
    std::cout << "Commande : '" << commande << "'\n";
    std::cout << "Paramètre : '" << param << "'\n";*/

    if (_channels.find(salon) == _channels.end())
    {
        std::cout << "Le salon " << salon << " n'existe pas" << std::endl;
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
                _channels[salon].addAdmin(nicknameToFd(param));
            else if (param != fdToNickname(user))
                _channels[salon].removeAdmin(nicknameToFd(param));
        }
        else
		{
            std::cout << param << " not found" << std::endl;
			std::string noParam = "NOTICE : Not enough parameters: <MODE> <channel> <parameter> <username>\r\n";
        	send(user, noParam.c_str(), noParam.size(), 0);
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
