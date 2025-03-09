/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:28:40 by kethouve          #+#    #+#             */
/*   Updated: 2025/03/07 22:10:22 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Channels.hpp"

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

/* Ajoute le user au canal */
int Channels::addUser(const int user, std::string added)
{
	if (!VerifUser(user) && !_invitMode)
		_user.push_back(user);
	else if (!VerifUser(user) && _invitMode)
	{
		if (VerifInvited(user))
		{
			_user.push_back(user);
			removeInvited(user);
		}
		else
		{
			std::string message = "this channel is on invitation and you don't have an invitation\n";
			send(user, message.c_str(), message.size(), 0);
			std::cout << MAGENTA << "[SERVER] " << RED << added << " is not invited on " << _channelName << RESET << std::endl;
			return 1;
		}
	}
	else
	{
			std::string message = "You are already in this channel\n";
			send(user, message.c_str(), message.size(), 0);
			std::cout << MAGENTA << "[SERVER] " << RED << added << " is already on " << _channelName << RESET << std::endl;
			return 1;
	}
	return 0;
}

/* Ajoute le user sur liste d'invitation */
void Channels::addInvited(const int user, std::string sender, std::string userInvited)
{
	if (!_invitMode || VerifInvited(user) || VerifUser(user))
	{
		std::cout << MAGENTA << "[SERVER] " << RED << "Error inviting :\nInvite mode = " << _invitMode << " \nVerifInvited = " << VerifInvited(user) << " \nVerifUser = " << VerifUser(user) << RESET << std::endl;
		return;
	}
	
	this->_invited.push_back(user);
	
	std::string invited = ":server Vous avez ete invite sur le salon '" + _channelName + "' \r\n";
    send(user, invited.c_str(), invited.size(), 0);
	std::cout << MAGENTA << "[SERVER] " << GREEN << sender << " a invité " << userInvited << " sur le salon " << _channelName << RESET << std::endl;
}

/* Passe le user en admin du canal */
void Channels::addAdmin(int userFd, std::string sender, std::string invited)
{
	if (!VerifAdmin(userFd))
	{
		_admins.push_back(userFd);
		std::string admined = ":server Vous avez ete promu admin sur '" + _channelName + "' ! \r\n";
    	send(userFd, admined.c_str(), admined.size(), 0);
		std::cout << MAGENTA << "[SERVER] " << GREEN << invited << " est promu admin sur le salon " << _channelName << "par " << sender << RESET << std::endl;
	}
	else
		std::cout << MAGENTA << "[SERVER] " << RED << invited << " est deja admin" << RESET << std::endl;
}

/* Supprime le user de la liste d'invitation */
void Channels::removeInvited(const int user)
{
	if (VerifInvited(user))
		_invited.erase(std::remove(_invited.begin(), _invited.end(), user), _invited.end());
}

/* Supprime le user de la liste des admins */
void Channels::removeAdmin(const int user)
{
	if (VerifAdmin(user))
		_admins.erase(std::remove(_admins.begin(), _admins.end(), user), _admins.end());
}

/* Supprime le user du canal */
void Channels::deleteUser(const int user)
{
	removeAdmin(user);
	removeInvited(user);
	if (VerifUser(user))
		_user.erase(std::remove(_user.begin(), _user.end(), user), _user.end());
}

/* Getters */
bool Channels::getInvitMode()
{
	return this->_invitMode;
}

int Channels::getUserLimit()
{
	return this->_userLimit;
}

bool Channels::getRestrictedTopic()
{
	return this->_restrictedTopic;
}

std::vector<int> Channels::getUsers() const
{
	return this->_user;
}

std::vector<int> Channels::getAdmins() const
{
	return this->_admins;
}

std::string Channels::getTopic() const
{
	return this->_channelTopic;
}

std::string Channels::getPassword() const
{
	return this->_channelPass;
}

std::string Channels::getChannelName() const
{
	return this->_channelName;
}

/* Setters */
void Channels::setUserLimit(const int limit, int user)
{
	this->_userLimit = limit;
	std::ostringstream ss;
	ss << limit;
	std::string newlimit = ss.str();
	std::cout << MAGENTA << "[SERVER] " << GREEN << this->_channelName << " new user limit is " << this->_userLimit << RESET << std::endl;
	if (this->_userLimit != INT_MAX)
	{
		std::string Seted = "NOTICE" + this->_channelName + " : user limit is set at: " + newlimit + "\r\n";
    	send(user, Seted.c_str(), Seted.size(), 0);
	}
	else
	{
		std::string Seted = "NOTICE" + this->_channelName + " : user limit is unset " + "\r\n";
    	send(user, Seted.c_str(), Seted.size(), 0);
	}
}

void Channels::setTopic(const std::string newTopic, int user)
{
	this->_channelTopic = newTopic;
	std::cout << this->_channelName << " new topic is " << this->_channelTopic << RESET << std::endl;
	std::cout << MAGENTA << "[SERVER] " << GREEN << "Nouveau topic du salon " << this->_channelName << ": " << _channelTopic << RESET << std::endl;
	std::string Seted = "NOTICE" + this->_channelName + " topic is now: " + this->_channelTopic + "\r\n";
    send(user, Seted.c_str(), Seted.size(), 0);
}

void Channels::setChannelPass(const std::string password, int user)
{
	this->_channelPass = password;
	if (!password.empty())
	{
		std::cout << MAGENTA << "[SERVER] " << GREEN << "New pass: " << this->_channelPass << RESET << std::endl;
		std::cout << MAGENTA << "[SERVER] " << GREEN << "Password '" << password << "' set for channel '" << this->_channelName << "'" << RESET << std::endl;
		std::string Seted = "NOTICE" + this->_channelName + " : has a password set\r\n";
        send(user, Seted.c_str(), Seted.size(), 0);
	}
	else
	{
		std::cout << MAGENTA << "[SERVER] " << GREEN << "Channel '" << this->_channelName << "' has no password" << RESET << std::endl;
		std::string Seted = "NOTICE" + this->_channelName + " : password unset\r\n";
        send(user, Seted.c_str(), Seted.size(), 0);
	}
}

void Channels::setInvitationMode(const std::string option, int user)
{
	if (option == "+i")
	{
		if (_invitMode)
		{
			std::cout << MAGENTA << "[SERVER] " << RED << "channel already set on ivitation mode" << RESET << std::endl;
			std::string alreadySet = "NOTICE " + this->_channelName + " : already set on ivitation mode\r\n";
        	send(user, alreadySet.c_str(), alreadySet.size(), 0);
			return;
		}
		else
		{
			this->_invitMode = true;
			std::cout << MAGENTA << "[SERVER] " << GREEN << this->_channelName << " is now on invitation mode" << RESET << std::endl;
			std::string Seted = "NOTICE" + this->_channelName + " : is now on invitation mode\r\n";
        	send(user, Seted.c_str(), Seted.size(), 0);
		}
	}
	else if (option == "-i")
	{
		if (_invitMode == false)
		{
			std::cout << MAGENTA << "[SERVER] " << RED << "channel is not on ivitation mode" << RESET << std::endl;
			std::string Seted = "NOTICE" + this->_channelName + " : is not on invitation mode\r\n";
        	send(user, Seted.c_str(), Seted.size(), 0);
			return;
		}
		else
		{
			this->_invitMode = false;
			std::cout << MAGENTA << "[SERVER] " << GREEN << this->_channelName << " invitation mode desactivated" << RESET << std::endl;
			std::string Seted = "NOTICE" + this->_channelName + " : is no more on invitation mode\r\n";
        	send(user, Seted.c_str(), Seted.size(), 0);
		}
	}
}

void Channels::setRestrictedTopic(const std::string option, int user, std::string nickName)
{
	if (option == "+t")
	{
		if (_restrictedTopic)
		{
			std::cout << MAGENTA << "[SERVER] " << RED << "topic already set on admin only" << RESET << std::endl;
			std::string alreadySet = "NOTICE " + nickName + " : Topic is already set on admin only\r\n";
        	send(user, alreadySet.c_str(), alreadySet.size(), 0);
			return;
		}
		else
		{
			this->_restrictedTopic = true;
			std::cout << MAGENTA << "[SERVER] " << GREEN << this->_channelName << " topic is now on admin only" << RESET << std::endl;
			std::string alreadySet = "NOTICE " + nickName + " : Topic is now set on admin only\r\n";
        	send(user, alreadySet.c_str(), alreadySet.size(), 0);
		}
	}
	else if (option == "-t")
	{
		if (_restrictedTopic == false)
		{
			std::cout << MAGENTA << "[SERVER] " << RED << "topic is already not on admin only" << RESET << std::endl;
			std::string alreadySet = "NOTICE " + nickName + " : Topic is already not on admin only\r\n";
        	send(user, alreadySet.c_str(), alreadySet.size(), 0);
			return;
		}
		else
		{
			this->_restrictedTopic = false;
			std::cout << MAGENTA << "[SERVER] " << GREEN << this->_channelName << " topic can be change by everyone now" << RESET << std::endl;
			std::string alreadySet = "NOTICE " + nickName + " : Topic is no more on admin only\r\n";
        	send(user, alreadySet.c_str(), alreadySet.size(), 0);
		}
	}
}
