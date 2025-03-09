/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 13:58:50 by kethouve          #+#    #+#             */
/*   Updated: 2025/03/07 22:13:02 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Server.hpp"

/*Constructor*/
Server::Server(int port, std::string pass)
{
	_serverPass = pass;
	_serverPort = port;
	_serverfd = 0;
	_serverfd = socket(AF_INET, SOCK_STREAM, 0);
}

Server::~Server()
{}

void	Server::setServerSocket()
{
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverAddr.sin_port = htons(_serverPort);
    int opt = 1;
    setsockopt(_serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bind(_serverfd, (sockaddr*)&_serverAddr, sizeof(_serverAddr));
    listen(_serverfd, 10);
    std::cout << MAGENTA << "[SERVER]" << GREEN << " Serveur en écoute sur le port " << _serverPort << "...\n" << RESET;
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
	else if (_user[pollFds[i].fd].user == true && _user[pollFds[i].fd].nick == true && _user[pollFds[i].fd].pass == false)
	{
		std::string errMessage = "[NOTICE] :" + _user[pollFds[i].fd].getUserNickName() + " echec de connection. Déconnection en cours\r\n";
        send(pollFds[i].fd, errMessage.c_str(), errMessage.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << GREEN << "Client : " << pollFds[i].fd << " " << _user[pollFds[i].fd].getUserNickName() << ": Echec de connection!" << RESET << std::endl;
		destroyUser(pollFds[i].fd);
	}
	if (_user[pollFds[i].fd].user == true && _user[pollFds[i].fd].nick == true && _user[pollFds[i].fd].pass == true)
	{
		_user[pollFds[i].fd].is_user = true;
		_channels["#general"].addUser(pollFds[i].fd, _user[pollFds[i].fd].getUserNickName());
		std::cout << MAGENTA << "[SERVER]" << YELLOW << " new user " << _user[pollFds[i].fd].getUserFd() << " with nickname: " << _user[pollFds[i].fd].getUserNickName() << RESET << std::endl;
	}
}

void Server::serverLoop()
{
    pollfd serverPollFd;
    serverPollFd.fd = _serverfd;
    serverPollFd.events = POLLIN;
    serverPollFd.revents = 0;
    pollFds.push_back(serverPollFd);
    _channels["#general"] = Channels("#general", 0);
    User newUser(0);
    _user[0] = newUser;
    char buffer[BUFFER_SIZE];
    while (true)
    {
        int eventCount = poll(&pollFds[0], pollFds.size(), -1);
        if (eventCount <= 0)
        {
            std::cerr << MAGENTA << "[SERVER]" << RED << " Erreur ou timeout dans poll()\n" << RESET;
            break;
        }
        for (size_t i = 0; i < pollFds.size(); ++i)
		{
            if (pollFds[i].revents & POLLIN)
			{
                if (pollFds[i].fd == _serverfd)
                {
                    int clientSocket = accept(_serverfd, NULL, NULL);
                    if (clientSocket != -1) {
                        std::cout << MAGENTA << "[SERVER]" << BLUE << " Nouvelle connection acceptée : " << clientSocket << "\n" << RESET;
                        fcntl(clientSocket, F_SETFL, O_NONBLOCK);

                        pollfd clientPollFd;
                        clientPollFd.fd = clientSocket;
                        clientPollFd.events = POLLIN;
                        clientPollFd.revents = 0;
                        pollFds.push_back(clientPollFd);
                        User newUser(clientSocket);
                        _user[clientSocket] = newUser;
						clientBuffers[clientSocket] = "";
                    }
                }
                else
                {
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytesReceived = recv(pollFds[i].fd, buffer, BUFFER_SIZE, 0);
                    if (bytesReceived <= 0)
                    {
                        destroyUser(pollFds[i].fd);
						clientBuffers.erase(pollFds[i].fd);
                        --i;
						continue;
                    }
                    clientBuffers[pollFds[i].fd] += std::string(buffer, bytesReceived);
					size_t pos;
                    while ((pos = clientBuffers[pollFds[i].fd].find("\n")) != std::string::npos) 
					{
						std::string message = clientBuffers[pollFds[i].fd].substr(0, pos + 1);
						clientBuffers[pollFds[i].fd].erase(0, pos + 1);
						if (!message.empty() && message[message.size() - 1] == '\r')
    						message.erase(message.size() - 1);
						if (message.empty())
							continue;
						if (_user[pollFds[i].fd].is_user == false)
						{
							userConnection(message, i);
						}
						else if (isCommand(message))
						{
							std::cout << CYAN << "[" << _user[pollFds[i].fd].getUserNickName() << "] " << RESET << BOLD << message << RESET;
							executeCommand(message, i);
						}
						else
						{
							std::ostringstream broadcastStream;
							broadcastStream << "Client " << pollFds[i].fd << " " << _user[pollFds[i].fd].getUserNickName() << ": " << message;
							std::cout << CYAN << "[" << _user[pollFds[i].fd].getUserNickName() << "] " << RESET << BOLD << message << RESET;
							std::string broadcastMessage = broadcastStream.str();
							_channels["#general"].sendMessage(broadcastMessage, pollFds[i].fd);
						}
					}
                }
            }
        }
    }
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
	close(_serverfd);
}
