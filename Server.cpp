/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 13:58:50 by kethouve          #+#    #+#             */
/*   Updated: 2025/02/22 03:58:09 by kethouve         ###   ########.fr       */
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

void	Server::userConnection(std::string message, int i)
{
	std::vector<std::string> args = parseMessage(message);
	if (message.find("CAP") == 0)
		return;
	else if (message.find("PASS") == 0)
	{
		pass(args, i);
	}
	else if (message.find("USER") == 0)
	{
		user(args, i);
	}
	else if (message.find("NICK") == 0)
	{
		nick(args, i);
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

void Server::serverLoop()
{
	std::cout << "in loop" << std::endl;
    // 6. Utiliser poll() pour surveiller les clients
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
                    }
                }
				// Message d'un user
                else
                {
					// Parsing message client (salon et contenu)
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytesReceived = recv(pollFds[i].fd, buffer, BUFFER_SIZE, 0);
					// Si message vide, déconnection
                    if (bytesReceived <= 0)
                    {
                        destroyUser(pollFds[i].fd);
						clientBuffers.erase(pollFds[i].fd);
                        --i; // Ajuster l'index après suppression
						continue;
                    }
					// Concaténer les nouvelles données avec celles précédemment reçues (gestion Ctrl+D)
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
						userConnection(message, i);
                    }
					
                    else if (isCommand(message))
                    {
						executeCommand(message, i);
					}
                    else
                    {
                        std::ostringstream broadcastStream;
                        broadcastStream << "Client " << pollFds[i].fd << " " << _user[pollFds[i].fd].getUserNickName() << ": " << message;
                        std::string broadcastMessage = broadcastStream.str();
                        _channels["#general"].sendMessage(broadcastMessage, pollFds[i].fd);
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
	clientBuffers.clear();
    pollFds.clear();
	std::vector<pollfd> empty;
    pollFds.swap(empty);
	std::cout << "all channels cleared" << std::endl;
	close(_serverfd);
}
