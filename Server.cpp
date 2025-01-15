/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 13:58:50 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/14 19:10:24 by kethouve         ###   ########.fr       */
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

void Server::serverLoop()
{
	std::cout << "in loop" << std::endl;
// 6. Utiliser poll() pour surveiller les clients
    std::vector<pollfd> pollFds;
    pollfd serverPollFd;
    serverPollFd.fd = _serverfd;
    serverPollFd.events = POLLIN;
    serverPollFd.revents = 0;
    pollFds.push_back(serverPollFd); // Surveiller le serveur.
    Channels nouveausalons("#general", 0);
    _channels["#general"] = nouveausalons;
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
				// Nouvelle connextion
                if (pollFds[i].fd == _serverfd)
                {
                    // Accepter une nouvelle connexion
                    int clientSocket = accept(_serverfd, NULL, NULL);
                    if (clientSocket != -1) {
                        std::cout << "Nouvelle connexion acceptée : " << clientSocket << "\n";
                        fcntl(clientSocket, F_SETFL, O_NONBLOCK); // Non bloquant.

                        pollfd clientPollFd;
                        clientPollFd.fd = clientSocket;
                        clientPollFd.events = POLLIN;
                        clientPollFd.revents = 0;
                        pollFds.push_back(clientPollFd); // Ajouter le client
                        User newUser(clientSocket);
                        _user[clientSocket] = newUser;
                        _channels["#general"].addUser(clientSocket);
                        for (std::map<int, User>::iterator it = _user.begin(); it != _user.end(); ++it)
                        {
                            std::cout << "user :" <<it->second.getUserFd() << "\n";
                        }
                    }
                }
				// Message d'un user
                else
                {
					// Parsing message client (salon et contenu)
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytesReceived = recv(pollFds[i].fd, buffer, BUFFER_SIZE, 0);
                    std::string message(buffer);
                    std::string salon;
                    std::string temp;
                    std::string msgContent;
                    std::ostringstream ss;
                    ss << pollFds[i].fd;
                    std::string userfd = ss.str();

					// Si message vide, déconnection
                    if (bytesReceived <= 0)
                    {
                        std::cout << "Client déconnecté : " << pollFds[i].fd << "\n";
                        close(pollFds[i].fd);
                        pollFds.erase(pollFds.begin() + i);
                        //_user.erase(std::remove(_user.begin(), _user.end(), userKick), _user.end());    <== SUPPRIMER LE USER
                        --i; // Ajuster l'index après suppression
						continue;
                    }
                    if (message.find("PRIVMSG") == 0)
                    {
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
                            std::ostringstream broadcastStream;
                            broadcastStream << "Client " << pollFds[i].fd << _user[i].getUserNickName() << ": " << msgContent;
                            std::string broadcastMessage = broadcastStream.str();
                            _channels[salon].sendMessage(broadcastMessage, pollFds[i].fd); //Probleme de salon actuelle
                        }
                        else
                        {
                            std::ostringstream broadcastStream;
                            broadcastStream << "Client " << pollFds[i].fd << _user[i].getUserNickName() << ": " << msgContent;
                            std::string broadcastMessage = broadcastStream.str();
                            int userfd;
                            std::istringstream ss(salon);
                            ss >> userfd;
                            if (ss.fail())
                            {
                                bool userFound = false;
                                for (std::map<int, User>::iterator it = _user.begin(); it != _user.end(); ++it)
                                {
                                    if (it->second.getUserNickName() == salon)
                                    {
                                        userfd = it->first;
                                        userFound = true;
                                        return;
                                    }
                                }
                                if (!userFound)
                                {
                                    std::cerr << "Utilisateur avec nickname ou ID " << salon << " introuvable.\n";
                                    return;
                                }
                            }
                            send(userfd, broadcastMessage.c_str(), broadcastMessage.size(), 0);
                        }
                    }
					// Recherche de commandes
                    else if (message.find("JOIN") == 0)
                    {
                        std::cout << "JOIN\n";
                        std::string joinSalon = message.substr(5);
                        joinSalon.erase(joinSalon.find_last_not_of(" \t\n\r") + 1);
                        if (joinSalon.empty())
                        {
                            std::cerr << "Erreur : nom de salons vide reçu.\n";
                            std::string errMessage = ":server 461 Client X JOIN :Pas assez de paramètres.\n";
                            send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
                            continue;
                        }
						// Creation de salon
                        if (_channels.find(joinSalon) == _channels.end())
                        {
							std::cout << "salon " << joinSalon << " created" << std::endl;
                            Channels nouveausalons(joinSalon, pollFds[i].fd);
                            _channels[joinSalon] = nouveausalons;
							std::string joinMessage =  "Comme il n'existe pas encore, vous avez cree le salon " + joinSalon + "\n";
							send(pollFds[i].fd, joinMessage.c_str(), joinMessage.size(), 0);
							continue;
                        }
						// Ajoute l'user au salon
                        if (!_channels[joinSalon].VerifUser(pollFds[i].fd))
                        {
                            std::string joinMessage = ":Client " + userfd + " JOIN " + joinSalon + "\n";
                            _channels[joinSalon].sendMessage(joinMessage, pollFds[i].fd);
							_channels[joinSalon].addUser(pollFds[i].fd);
                            std::string topicMessage = ":server TOPIC " + userfd + " :Bienvenue dans " + joinSalon + "\n";
                            send(pollFds[i].fd, topicMessage.c_str(), topicMessage.size(), 0);
                        }
						else
							std::cout << "le user est deja dans le salon " << joinSalon << std::endl;
                    }
                    
                    else if (message.find("KICK") == 0)
                    {
                        std::cout << "KICK\n";
						// Recuperer le user a KICK
                        /*std::string userToKick = message.substr(5);
                        std::stringstream ss(userToKick);
                        int userKick;
                        ss >> userKick;
                        if (ss.fail())
                        {
                            // faire une fonction qui recup le fd a partir du nickname
                        }*/
					    std::string userKickName = message.substr(5);
                        userKickName.erase(userKickName.find_last_not_of(" \t\n\r") + 1);
						int userKick = std::atoi(userKickName.c_str());

						if (_channels[salon].VerifAdmin(pollFds[i].fd) && _channels[salon].VerifUser(userKick))
						{
                            std::string kickMessage = ":server " + salon + " " + (char)userKick + "was kick" + "\n";
                            _channels[salon].sendMessage(kickMessage, 0);
						}
                        else if (!_channels[salon].VerifAdmin(pollFds[i].fd))
                        {
                            std::string notAdmin = ":server " + salon + "no permission for kick" + "\n";
                            send(pollFds[i].fd, notAdmin.c_str(), notAdmin.size(), 0);
                        }  
                        else
                        {
                            std::string notAdmin = ":server " + salon + "user not found in channel" + "\n";
                            send(pollFds[i].fd, notAdmin.c_str(), notAdmin.size(), 0);
                        }  
                    }
                    else if (message.find("NICK") == 0)
                    {
                        std::string userNickName = message.substr(5);
                        userNickName.erase(userNickName.find_last_not_of(" \t\n\r") + 1);
                        _user[pollFds[i].fd].setUserNickName(userNickName);
                    }
					/*
                    else if (message.find("INVITE") == 0)
                    {
                    }
                    else if (message.find("TOPIC") == 0)
                    {
                    }
                    else if (message.find("MODE") == 0)
                    {
                    }*/


                    else
                    {
						salon = "#general";
                        //std::cout << "test msg envoyer\n";
						//std::cout << "salon: " << salon << std::endl;
                        //std::cout << "Message reçu de " << pollFds[i].fd << ": " << message << "\n";
                        std::ostringstream broadcastStream;
                        broadcastStream << "Client " << pollFds[i].fd << " " << _user[pollFds[i].fd].getUserNickName() << ": " << message;
                        std::string broadcastMessage = broadcastStream.str();
                        _channels[salon].sendMessage(broadcastMessage, pollFds[i].fd); //Probleme de salon actuelle
                    }
                }
            }
        }
    }

    // Fermer le serveur
    close(_serverfd);
}
