/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:28:40 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/15 16:10:58 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channels.hpp"

Channels::Channels(){}

Channels::Channels(std::string name, int fdCreator)
{
	this->_channelName = name;
	this->_userLimit = INT_MAX;
	this->_admins.push_back(fdCreator);
	this->_user.push_back(fdCreator);
}

Channels::~Channels(){}

void Channels::sendMessage(std::string message, int sender)
{
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

void Channels::deleteUser(const int user)
{
	if (VerifUser(user))
		_user.erase(std::remove(_user.begin(), _user.end(), user), _user.end());
	if (VerifAdmin(user))
		_user.erase(std::remove(_admins.begin(), _admins.end(), user), _admins.end());
}

void Channels::addAdmin(int userFd)
{
	if (!VerifAdmin(userFd))
		_admins.push_back(userFd);
}

void Channels::setUserLimit(const int limit)
{
	this->_userLimit = limit;
	std::cout << this->_channelName << " new user limit is " << this->_userLimit << std::endl;
}

void Channels::setChannelPass(const std::string password)
{
	this->_channelPass = password;
	std::cout << "New pass: " << this->_channelPass << std::endl;
	std::cout << "Password set for channel " << this->_channelName << std::endl;
}
