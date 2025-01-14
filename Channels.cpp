/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:28:40 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/14 15:33:37 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channels.hpp"

Channels::Channels(){}

Channels::Channels(std::string name, int fdCreator)
{
	this->_channelName = name;
	this->userLimit = INT_MAX;
	this->_admins.push_back(fdCreator);
	this->_user.push_back(fdCreator);
}

Channels::~Channels(){}

void Channels::sendMessage(std::string message, int sender)
{
	//std::vector<int>::iterator itv = _user.begin();
	//std::cout << message << sender << std::endl;
	for (std::vector<int>::iterator itv = _user.begin(); itv != _user.end(); itv++)
	{
		if (*itv != sender)
			send(*itv, message.c_str(), message.size(), 0);
	}
}

bool Channels::VerifAdmin(int userFd)
{
	for(std::vector<int>::iterator itv = _admins.begin(); itv != _admins.end(); itv++)
	{
		if (userFd == *itv)
			return true;
	}
	return false;
}

bool Channels::VerifUser(int userFd)
{
	for(std::vector<int>::iterator itv = _user.begin(); itv != _user.end(); itv++)
	{
		if (userFd == *itv)
			return true;
	}
	return false;
}

/*Getter*/
std::vector<int> Channels::getUsers() const
{
	return this->_user;
}

/*Setter*/
void Channels::addUser(const int user)
{
	if (!VerifUser(user))
		_user.push_back(user);

}

void Channels::addAdmin(int userFd)
{
	if (!VerifAdmin(userFd))
		_admins.push_back(userFd);
}

void Channels::setUserLimit(const int limit)
{
	this->userLimit = limit;
}

void Channels::setChannelPass(const std::string password)
{
	this->_channelPass = password;
}
