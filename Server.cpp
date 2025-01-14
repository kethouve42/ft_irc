/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 13:58:50 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/09 15:20:00 by kethouve         ###   ########.fr       */
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

    // 7. Boucle principale
    char buffer[BUFFER_SIZE];
    while (true) {
        int eventCount = poll(&pollFds[0], pollFds.size(), -1); // Attendre un événement.
        if (eventCount <= 0) {
            std::cerr << "Erreur ou timeout dans poll()\n";
            break;
        }

        for (size_t i = 0; i < pollFds.size(); ++i) {
            // Vérifier le socket du serveur
            if (pollFds[i].revents & POLLIN) {
                if (pollFds[i].fd == _serverfd) {
                    // Accepter une nouvelle connexion
                    int clientSocket = accept(_serverfd, NULL, NULL);
                    if (clientSocket != -1) {
                        std::cout << "Nouvelle connexion acceptée : " << clientSocket << "\n";
                        fcntl(clientSocket, F_SETFL, O_NONBLOCK); // Non bloquant.

                        pollfd clientPollFd;
                        clientPollFd.fd = clientSocket;
                        clientPollFd.events = POLLIN;
                        clientPollFd.revents = 0;
                        pollFds.push_back(clientPollFd); // Ajouter le client.
                        _user[i] = User newUser(i);
                    }
                } else {
                    // Recevoir des messages d'un client
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytesReceived = recv(pollFds[i].fd, buffer, BUFFER_SIZE, 0);
                    if (bytesReceived <= 0)
                    {
                        // Déconnexion
                        std::cout << "Client déconnecté : " << pollFds[i].fd << "\n";
                        close(pollFds[i].fd);
                        pollFds.erase(pollFds.begin() + i);
                        --i; // Ajuster l'index après suppression
                    }
                    std::string message(buffer);
                    size_t pos = message.find(" ");
                    std::string salon;
                    std::string msgContent;
                    if (pos != std::string::npos)
                    {
                        salon = message.substr(8, pos - 8);
                        msgContent = message.substr(pos + 1);
                    }
                    else if (message.find("JOIN") == 0)
                    {
                        std::string salons = message.substr(5);
                        salons.erase(salons.find_last_not_of(" \t\n\r") + 1);
                        if (salons.empty())
                        {
                            std::cerr << "Erreur : nom de salons vide reçu.\n";
                            std::string errMessage = ":server 461 Client" + std::to_string(i) + " JOIN :Pas assez de paramètres.\n";
                            send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
                            continue;
                        }
                        bool userTrouve = false;
                        if (_channels.find(salons) == _channels.end())
                            _channels[salons] = Channels nouveausalons(salons, i);
                        else
                            userTrouve = _channels[salons].VerifUsers(i);
                        if (!userTrouve)
                        {
                            std::string joinMessage = ":Client" + std::to_string(i) + " JOIN " + salons + "\n";
                            _channels[salons].sendMessage(joinMessage, i);
                            std::string topicMessage = ":server TOPIC " + salons + " :Bienvenue dans " + salons + "\n";
                            send(i, topicMessage.c_str(), topicMessage.size(), 0);
                            std::string namesMessage = ":server 353 Client" + std::to_string(i) + " = " + salons + " :";
                            std::vector<int> membre = _channels[salons].getUser();
                            for (size_t k = 0; k < membre.size(); ++k)
                            {
                                namesMessage += "Client" + std::to_string(membre[k]) + " ";
                            }
                            namesMessage += "\n:server 366 Client" + std::to_string(i) + " " + salons + " :End of /NAMES list.\n";
                            send(i, namesMessage.c_str(), namesMessage.size(), 0);
                        }
                    }
                    else if (message.find("KICK") == 0)
                    {
                        std::string userToKick = message.substr(5);
                        std::stringstream ss(userToKick);
                        int userKick;
                        ss >> userKick;
                        if (ss.fail())
                        {
                            // faire une fonction qui recup le fd a partir du nickname
                            userKick
                        }
                        bool adminTrouve = false;
                        bool userTrouve = _channels[salon].VerifUsers(userKick);
                        if (adminTrouve = _channels[salon].VerifAdmin(i) && userTrouve == true)
                        {
                            std::string kick = ":server " + salon + std::to_string(userToKick) + "was kick" + "\n";
                            _channels[salons].sendMessage(kick, 0);
                            _user.erase(std::remove(_user.begin(), _user.end(), userKick), _user.end());
                        }
                        else if (userTrouve == false)
                        {
                            std::string notAdmin = ":server " + salon + std::to_string(i) + "no user in channel" + "\n";
                            send(i, notAdmin.c_str(), notAdmin.size(), 0);
                        }  
                        else
                        {
                            std::string notAdmin = ":server " + salon + std::to_string(i) + "no permission for kick" + "\n";
                            send(i, notAdmin.c_str(), notAdmin.size(), 0);
                        }  
                    }
                    /*else if (message.find("INVITE") == 0)
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
                        std::cout << "Message reçu de " << pollFds[i].fd << ": " << message << "\n";
                        std::ostringstream broadcastStream;
                        broadcastStream << "Client " << _user[i].getUserNickName() << ": " << message;
                        std::string broadcastMessage = broadcastStream.str();
                        _channels[salon].sendMessage(msgContent, i);
                    }
                }
            }
        }
    }

    // Fermer le serveur
    close(_serverfd);
}
