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
                    }
                } else {
                    // Recevoir des messages d'un client
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytesReceived = recv(pollFds[i].fd, buffer, BUFFER_SIZE, 0);
                    if (bytesReceived <= 0) {
                        // Déconnexion
                        std::cout << "Client déconnecté : " << pollFds[i].fd << "\n";
                        close(pollFds[i].fd);
                        pollFds.erase(pollFds.begin() + i);
                        --i; // Ajuster l'index après suppression
                    } else {
                        std::cout << "Message reçu de " << pollFds[i].fd << ": " << buffer << "\n";

                        // Diffuser le message à tous les autres clients
                        std::ostringstream broadcastStream;
                        broadcastStream << "Client " << pollFds[i].fd << ": " << buffer;
                        std::string broadcastMessage = broadcastStream.str();

                        for (size_t j = 0; j < pollFds.size(); ++j) {
                            if (pollFds[j].fd != _serverfd && pollFds[j].fd != pollFds[i].fd) {
                                send(pollFds[j].fd, broadcastMessage.c_str(), broadcastMessage.size(), 0);
                            }
                        }
                    }
                }
            }
        }
    }

    // Fermer le serveur
    close(_serverfd);
}

/*Destructor*/
Server::~Server(){}