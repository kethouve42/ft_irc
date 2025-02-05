/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acasanov <acasanov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 13:58:50 by kethouve          #+#    #+#             */
/*   Updated: 2025/02/04 17:08:56 by acasanov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*Constructor*/
Server::Server(int port, std::string pass)
{
	_serverPass = pass;
	_serverPort = port;
	_serverfd = 0;
	_serverfd = socket(AF_INET, SOCK_STREAM, 0);
	/*_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY; //accepter toute les adresses
	_serverAddr.sin_port = htons(_serverPort);*/
}

Server::~Server()
{}

void	Server::setServerSocket()
{
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY; //accepter toute les adresses
	_serverAddr.sin_port = htons(_serverPort);
	// 3. Éviter les erreurs "port déjà utilisé"
    int opt = 1;
    setsockopt(_serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // Associer le socket à l'adresse (bind)
    bind(_serverfd, (sockaddr*)&_serverAddr, sizeof(_serverAddr));
    // 4. Commencer à écouter les connexions
    listen(_serverfd, 10);
    std::cout << "Serveur en écoute sur le port " << _serverPort << "...\n";

    // 5. Configurer le serveur en mode "non bloquant"
    fcntl(_serverfd, F_SETFL, O_NONBLOCK);
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

void Server::nick(std::string message, size_t i)
{
	std::string userNickName = message.substr(5);
    userNickName.erase(userNickName.find_last_not_of(" \t\n\r") + 1);
	if (userNickName.empty())
    {
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " NICK :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
        return;
    }
	if (!nickExist(userNickName))
	{
		_user[pollFds[i].fd].setUserNickName(userNickName);
		_user[pollFds[i].fd].nick = true;
	}
	else
	{
		std::string errMessage = "[SERVER]: " + userNickName + " existe deja trouvé un autre NickName!\r\n"; //trouver le code
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
		std::cout << userNickName << " existe deja trouvé un autre NickName!" << std::endl;
		return;
	}
}

void Server::privmsg(std::string message, size_t i) //Gestion des :
{
	std::string temp;
	std::string salon;
	std::string msgContent;
    std::cout << "PRIVMSG\n";
    size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        temp = message.substr(8, pos - 8);
        size_t pos = temp.find(" ");
        salon = temp.substr(0, pos);
        msgContent = temp.substr(pos + 1);
        std::cout << "msg : " << msgContent << "salon :" << salon << "\n";
    }
    if (salon[0] == '#')
    {
        std::stringstream messageStream;
        messageStream << ":" << _user[pollFds[i].fd].getUserNickName() << " PRIVMSG " 
                    << salon << " :" << msgContent << "\r\n";
        std::string messageToSend = messageStream.str();
        _channels[salon].sendMessage(messageToSend, pollFds[i].fd);
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

void Server::user(std::string message, size_t i)
{
	std::string username;
    std::string hostname;
    std::string servername;
    std::string realname;

    size_t start = 5;
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
    }
    if (realname.size() != 0) //Gestion des :
    {
        _user[pollFds[i].fd].setUserRealname(realname);
        _user[pollFds[i].fd].setUserName(username);
        _user[pollFds[i].fd].user = true;
    }
    else
    {
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " USER :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
        return;
    }
}

void Server::pass(std::string message, size_t i)
{
	std::string userPass = message.substr(5);
    userPass.erase(userPass.find_last_not_of(" \t\n\r") + 1);
    if (userPass.empty())
    {
        std::string errMessage = ":server 461 " + _user[pollFds[i].fd].getUserNickName() + " PASS :Not enough parameters\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
        return;
    }
    if (userPass[0] == ':')
    {
        if (":" + _serverPass == userPass)
            _user[pollFds[i].fd].pass = true;
        else
            destroyUser(pollFds[i].fd);
    }
    else
    {
        if (_serverPass == userPass)
            _user[pollFds[i].fd].pass = true;
        else
            destroyUser(pollFds[i].fd);
    }
}

void Server::displayUsers(std::string salon, int sender)
{
    std::string message = salon + "'s users :\n";
    for (std::map<int, User>::iterator it = _user.begin(); it != _user.end(); ++it)
	{
        if (_channels[salon].VerifUser(it->second.getUserFd()))
            message += it->second.getUserNickName() + "\n";
	}
	send(sender, message.c_str(), message.size(), 0);
}

void Server::destroyUser(const int user)
{
    std::cout << "Client déconnecté : " << user << "\n";
    for (std::map<std::string, Channels>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        it->second.deleteUser(user);
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
		std::cout << RED << "Fermeture du salon " << salon << " faute d'utilisateur" << RESET << std::endl;
	}
}

/* Ajoute le user au salon */
void Server::join(std::string message, int user) //Ajouter une verif pour le pass JOIN [salon] [pass]
{
    std::string joinSalon;
    std::string passWord;
    std::string nickname = _user[user].getUserNickName();

    size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        std::string temp = message.substr(5);
        size_t pos = temp.find(" ");
        joinSalon = temp.substr(0, pos);
        passWord = temp.substr(pos + 1);
    }
    joinSalon.erase(joinSalon.find_last_not_of(" \t\n\r") + 1);
    passWord.erase(passWord.find_last_not_of(" \t\n\r") + 1);

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
        std::string endNamesMessage = ":server 366 " + nickname + " " + joinSalon + " :End of /NAMES list\r\n";

        send(user, namesMessage.c_str(), namesMessage.size(), 0);
        send(user, endNamesMessage.c_str(), endNamesMessage.size(), 0);

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
void Server::kick(std::string message, int user)
{
	std::string userKickName;
    std::string salon;

	size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        std::string temp = message.substr(5);
        size_t pos = temp.find(" ");
        salon = temp.substr(0, pos);
        userKickName = temp.substr(pos + 1);
    }

    userKickName.erase(userKickName.find_last_not_of(" \t\n\r") + 1);
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
	if (_channels[salon].VerifAdmin(user)/* && userKickName != fdToNickname(user)*/)        // Possible de s'auto kick
	{
        std::string kickMessage = BLUE + ":server " + RESET + _user[userKick].getUserNickName() + " was kick out of " + salon + "\n";
        _channels[salon].sendMessage(kickMessage, 0);
		_channels[salon].deleteUser(userKick);
		destroyChannel(salon);
	}
    else if (!_channels[salon].VerifAdmin(user))
    {
        std::string notAdmin = RED + ":server " + RESET + salon + " no permission for kick" + "\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
    }
    else
    {
        std::string notAdmin = RED + ":server " + RESET + "error" + "\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
    }  
}

/* Afficher ou changer le sujet d'un salon */
void Server::topic(std::string message, int user)
{
	std::string newTopic;
    std::string salon;

    message.erase(0, message.find_first_not_of(" \t\n\r"));
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
    }

    if (_channels.find(salon) == _channels.end())
    {
        std::cout << "Le salon " << salon << " n'existe pas" << std::endl;
        return;
    }

    if (_channels[salon].getInvitMode() && !_channels[salon].VerifUser(user))
    {
        std::string notMember = RED + ":server " + RESET + salon + " topic avaliable only to members\n";
        send(user, notMember.c_str(), notMember.size(), 0);
        return;
    }

    if (newTopic.empty() && !_channels[salon].getTopic().empty())
    {
        std::string topicMessage = BLUE + ":server " + RESET + "TOPIC of " + salon + " : " + _channels[salon].getTopic() + "\n";
        send(user, topicMessage.c_str(), topicMessage.size(), 0);
    }
    else if (newTopic.empty() && _channels[salon].getTopic().empty())
    {
        std::string noTopic = BLUE + ":server " + RESET + salon + " n'a pas encore de topic\n";
        send(user, noTopic.c_str(), noTopic.size(), 0);
    }
    else if (_channels[salon].getRestrictedTopic() && !_channels[salon].VerifAdmin(user))
    {
        std::string notAdmin = RED + ":server " + RESET + salon + " no permission for change topic\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
    }
    else
    {
        _channels[salon].setTopic(newTopic);

        std::string newTopicMessage = BLUE + ":server " + RESET + "New topic on " + salon + " : " + newTopic + "\n";
        _channels[salon].sendMessage(newTopicMessage, user);
    }
}

void Server::invite(std::string message, int user)
{
    std::string userInviteName;
    std::string salon;

	size_t pos = message.find(" ");
    if (pos != std::string::npos)
    {
        std::string temp = message.substr(7);
        size_t pos = temp.find(" ");
        salon = temp.substr(0, pos);
        userInviteName = temp.substr(pos + 1);
    }

    if (_channels.find(salon) == _channels.end())
    {
        std::cout << "Le salon " << salon << " n'existe pas" << std::endl;
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
            return;
        }
	}

    if (_channels[salon].VerifUser(user))
        _channels[salon].addInvited(userInvite);
    else
        std::cout << "user n'est meme pas membre, et ne peux donc pas inviter\n";

}

/* Change les parametres du salon */
void Server::mode(std::string message, int user)
{
	std::string commande;
    std::string salon;
    std::string param;

    message.erase(0, message.find_first_not_of(" \t\n\r"));
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
    }
    // Debug : Afficher les parties extraites
    /*std::cout << "Salon : '" << salon << "'\n";
    std::cout << "Commande : '" << commande << "'\n";
    std::cout << "Paramètre : '" << param << "'\n";*/

    if (_channels.find(salon) == _channels.end())
    {
        std::cout << "Le salon " << salon << " n'existe pas" << std::endl;
        return;
    } 
    else if (!_channels[salon].VerifAdmin(user))
    {
        std::string notAdmin = RED + ":server " + RESET + salon + " only admins can use MODE (so you're not)\n";
        send(user, notAdmin.c_str(), notAdmin.size(), 0);
        return;
    }

    if (commande == "+i" || commande == "-i")
    {
        _channels[salon].setInvitationMode(commande);
    }
    else if (commande == "+t" || commande == "-t")
    {
        _channels[salon].setRestrictedTopic(commande);
    }
    else if (commande == "+k" && !param.empty())
    {
        _channels[salon].setChannelPass(param);
    }
    else if (commande == "-k")
    {
        _channels[salon].setChannelPass("");
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
            std::cout << param << " not found" << std::endl;
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
            _channels[salon].setUserLimit(value);
    }
    else if (commande == "-l")
    {
        _channels[salon].setUserLimit(INT_MAX);
    }
}

/* Retire le user du salon */
void Server::quit(std::string message, int user)
{
    std::string quitSalon = message.substr(5);
    quitSalon.erase(quitSalon.find_last_not_of(" \t\n\r") + 1);

    if (quitSalon.empty())
    {
        std::string errMessage = RED + ":server" + RESET + " Pas assez de paramètres.\n";
        send(user, errMessage.c_str(), errMessage.size(), 0);
        return ;
    }

    if (_channels.find(quitSalon) == _channels.end())
    {
        std::cout << "Le salon " << quitSalon << " n'existe pas" << std::endl;
        return;
    }

    if (_channels[quitSalon].VerifAdmin(user))
    {
		// Possible de quitter le salon même si on est le dernier modé


        /*if (_channels[quitSalon].getAdmins().size() <= 1 && _channels[quitSalon].getUsers().size() > 1)
        {
            std::string errMessage = RED + ":server" + RESET + " Il semble que vous etes le seul admin du serveur.\n\t Promouvez un autre membre avec 'MODE " + quitSalon + " +o user' avant de partir\n";
            send(user, errMessage.c_str(), errMessage.size(), 0);
            return ;
        }*/
    }

    _channels[quitSalon].deleteUser(user);
	destroyChannel(quitSalon); // Verifier le message a envoyé a Konversation
    std::string quitMessage = BLUE + ":server " + RESET + fdToNickname(user) + " a quitte le salon '" + quitSalon + "'\n";
    _channels[quitSalon].sendMessage(quitMessage, user);
    return;
}

///////////////////////////////////////
///////   Gerer le Ctrl+D   ///////////
///////////////////////////////////////
void Server::serverLoop()
{
	std::cout << "in loop" << std::endl;
    // 6. Utiliser poll() pour surveiller les clients
    //std::vector<pollfd> pollFds;
    pollfd serverPollFd;
    serverPollFd.fd = _serverfd;
    serverPollFd.events = POLLIN;
    serverPollFd.revents = 0;
    pollFds.push_back(serverPollFd); // Surveiller le serveur.
    _channels["#general"] = Channels("#general", 0);
    User newUser(0);
    _user[0] = newUser;
    // 7. Boucle principale
    char buffer[BUFFER_SIZE];
    while (true)
    {
        int eventCount = poll(&pollFds[0], pollFds.size(), -1); // Attendre un événement.
        if (eventCount <= 0)
        {
            std::cerr << "Erreur ou timeout dans poll()\n";
            break;
        }
        for (size_t i = 0; i < pollFds.size(); ++i)
		{
            // Vérifier le socket du serveur
            if (pollFds[i].revents & POLLIN)
			{
				// Nouvelle connection
                if (pollFds[i].fd == _serverfd)
                {
                    // Accepter une nouvelle connection
                    int clientSocket = accept(_serverfd, NULL, NULL);
                    if (clientSocket != -1) {
                        std::cout << "Nouvelle connection acceptée : " << clientSocket << "\n";
                        fcntl(clientSocket, F_SETFL, O_NONBLOCK); // Non bloquant.

                        pollfd clientPollFd;
                        clientPollFd.fd = clientSocket;
                        clientPollFd.events = POLLIN;
                        clientPollFd.revents = 0;
                        pollFds.push_back(clientPollFd); // Ajouter le client
                        User newUser(clientSocket);
                        _user[clientSocket] = newUser;
						clientBuffers[clientSocket] = ""; // Initialiser le buffer du client
                        _channels["#general"].addUser(clientSocket);
                    }
                }
				// Message d'un user
                else
                {
					// Parsing message client (salon et contenu)
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytesReceived = recv(pollFds[i].fd, buffer, BUFFER_SIZE, 0);
                    /*std::string message(buffer);
                    std::cout << message;*/
                    std::ostringstream ss;
                    ss << pollFds[i].fd;
                    std::string userfd = ss.str();
					// Si message vide, déconnection
                    if (bytesReceived <= 0)
                    {
                        destroyUser(pollFds[i].fd);
						clientBuffers.erase(pollFds[i].fd);
                        --i; // Ajuster l'index après suppression
						continue;
                    }
					// Concaténer les nouvelles données avec celles précédemment reçues
                    clientBuffers[pollFds[i].fd] += std::string(buffer, bytesReceived);
					std::string message;
					size_t pos;
                    while ((pos = clientBuffers[pollFds[i].fd].find("\n")) != std::string::npos) 
					{
						message = (clientBuffers[pollFds[i].fd].substr(0, pos + 1));
                        clientBuffers[pollFds[i].fd].erase(0, pos + 1);
					}
					if (message.empty())
						continue;
                    std::cout << message;
                    if (_user[pollFds[i].fd].is_user == false)
                    {
                        if (message.find("CAP") == 0)
                            continue;
                        else if (message.find("PASS") == 0)
                        {
							pass(message, i);
                        }
                        else if (message.find("USER") == 0)
                        {
							user(message, i);
                        }
                        else if (message.find("NICK") == 0)
                        {
                            nick(message, i);
                        }
                        if (_user[pollFds[i].fd].user == true && _user[pollFds[i].fd].nick == true && _user[pollFds[i].fd].pass == false)
                            destroyUser(pollFds[i].fd);
                        if (_user[pollFds[i].fd].user == true && _user[pollFds[i].fd].nick == true && _user[pollFds[i].fd].pass == true)
                        {
                            _user[pollFds[i].fd].is_user = true;
                            _channels["#general"].addUser(pollFds[i].fd);
                            std::cout << "new user " << _user[pollFds[i].fd].getUserFd() << "\n";
                        }
                    }
                    else
                    {
                        if (message.find("PRIVMSG") == 0)
                        {
							privmsg(message, i);
                        }
                        else if (message.find("JOIN") == 0)
                        {
                            join(message, pollFds[i].fd);
                        }
                        else if (message.find("KICK") == 0)
                        {
                            kick(message, pollFds[i].fd);
                        }
                        else if (message.find("NICK") == 0)
                        {
                            nick(message, i);
                        }
                        else if (message.find("TOPIC") == 0)
                        {
                            topic(message, pollFds[i].fd);
                        }
                        else if (message.find("INVITE") == 0)
                        {
                            invite(message, pollFds[i].fd);
                        }
                        else if (message.find("MODE") == 0)
                        {
                            mode(message, pollFds[i].fd);
                        }
                        else if (message.find("QUIT") == 0)
                        {
                            quit(message, pollFds[i].fd);
                        }
                        else if (message.find("USER") == 0)
                        {
                            user(message, pollFds[i].fd);
                        }
                        else if (message.find("DISPLAYUSER") == 0) // <== DEBUG
                        {
                            std::string salon = message.substr(12);
                            salon.erase(salon.find_last_not_of(" \t\n\r") + 1);
                            displayUsers(salon , pollFds[i].fd);
                        }
                        
                        else
                        {
                            std::string gene = "#general";
                            std::ostringstream broadcastStream;
                            broadcastStream << "Client " << pollFds[i].fd << " " << _user[pollFds[i].fd].getUserNickName() << ": " << message;
                            std::string broadcastMessage = broadcastStream.str();
                            _channels[gene].sendMessage(broadcastMessage, pollFds[i].fd);
                        }
                    }
                }
            }
        }
    }

    // Fermer le serveur
    close(_serverfd);
}

void Server::clearServ()
{
	_channels.clear();
	_user.clear();
    pollFds.clear();
	std::vector<pollfd> empty;
    pollFds.swap(empty);
	std::cout << "all channels cleared" << std::endl;
	close(_serverfd);
}
