/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acasanov <acasanov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:28:40 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/20 19:18:29 by acasanov         ###   ########.fr       */
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
	_invitMode = false;
	_restrictedTopic = false;
}

Channels::~Channels(){}

/* Envoie un message a tout les users du canal, sauf l'envoyeur */
void Channels::sendMessage(std::string message, int sender)
{
	for (std::vector<int>::iterator itv = _user.begin(); itv != _user.end(); itv++)
	{
		if (*itv != sender)
			send(*itv, message.c_str(), message.size(), 0);
	}
}

/* Verifie si le user est admin sur le canal */
bool Channels::VerifAdmin(int userFd)
{
	for(std::vector<int>::iterator itv = _admins.begin(); itv != _admins.end(); itv++)
	{
		if (userFd == *itv)
			return true;
	}
	return false;
}

/* Verifie si le user est deja present sur le canal */
bool Channels::VerifUser(int userFd)
{
	for(std::vector<int>::iterator itv = _user.begin(); itv != _user.end(); itv++)
	{
		if (userFd == *itv)
			return true;
	}
	return false;
}

/* Verifie si le user est sur la liste d'invitation du canal */
bool Channels::VerifInvited(const int user)
{
	for(std::vector<int>::iterator itv = _invited.begin(); itv != _invited.end(); itv++)
	{
		if (user == *itv)
			return true;
	}
	return false;
}

bool Channels::VerifInvitMode()
{
	return this->_invitMode;
}

std::vector<int> Channels::getUsers() const
{
	return this->_user;
}

std::string Channels::getTopic() const
{
	return this->_channelTopic;
}

/* Ajoute le user au canal */
void Channels::addUser(const int user)
{
	if (!VerifUser(user) && !VerifInvitMode())
		_user.push_back(user);
	else if (!VerifUser(user) && VerifInvitMode())
	{
		if (VerifInvited(user))
		{
			_user.push_back(user);
			removeInvited(user);
		}
		else
		{
			std::string message = "this channel is on invitation and you don't have an invitation";
			send(user, message.c_str(), message.size(), 0);
			return ;
		}
	}
	else
	{
			std::string message = "You are already in this channel";
			send(user, message.c_str(), message.size(), 0);
	}
}

/* Ajoute le user sur liste d'invitation */
void Channels::addInvited(const int user)
{
	if (!VerifInvitMode() || !VerifInvited(user) || VerifUser(user)) // et si il est deja dans le serv ?
		return;
	this->_invited.push_back(user);
}

/* Supprime le user de la liste d'invitation */
void Channels::removeInvited(const int user)
{
	if (VerifInvited(user))
		_invited.erase(std::remove(_invited.begin(), _invited.end(), user), _invited.end());
}

/* Supprime le user du canal */
void Channels::deleteUser(const int user)
{
	if (VerifUser(user))
		_user.erase(std::remove(_user.begin(), _user.end(), user), _user.end());
	if (VerifAdmin(user))
		_user.erase(std::remove(_admins.begin(), _admins.end(), user), _admins.end());
	if (VerifInvited(user))
		_user.erase(std::remove(_invited.begin(), _invited.end(), user), _invited.end());
}

/* Passe le user en admin du canal */
void Channels::addAdmin(int userFd)
{
	if (!VerifAdmin(userFd))
		_admins.push_back(userFd);
}


/* Setters */
void Channels::setUserLimit(const int limit)
{
	this->_userLimit = limit;
	std::cout << this->_channelName << " new user limit is " << this->_userLimit << std::endl;
}

void Channels::setTopic(const std::string newTopic)
{
	this->_channelTopic = newTopic;
	std::cout << this->_channelName << " new topic is " << this->_channelTopic << std::endl;
}

void Channels::setChannelPass(const std::string password)
{
	this->_channelPass = password;
	std::cout << "New pass: " << this->_channelPass << std::endl;
	std::cout << "Password set for channel " << this->_channelName << std::endl;
}

void Channels::setInvitationMode(const std::string option)
{
	if (option == "+i")
	{
		if (_invitMode)
		{
			std::cout << "channel already set on ivitation mode" << std::endl;
			return;
		}
		else
		{
			this->_invitMode = true;
			std::cout << this->_channelName << " is now on invitation mode" << std::endl;
		}
	}
	else if (option == "-i")
	{
		if (_invitMode == false)
		{
			std::cout << "channel is not on ivitation mode" << std::endl;
			return;
		}
		else
		{
			this->_invitMode = false;
			std::cout << this->_channelName << " invitation mode desactivated" << std::endl;
		}
	}
}
