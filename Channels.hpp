/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:27:28 by kethouve          #+#    #+#             */
/*   Updated: 2025/02/12 16:21:53 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"

class User;

class Channels
{
private:
	std::string 		_channelName;
	std::string 		_channelPass;
	std::string			_channelTopic;
	std::vector<int> 	_user;
	std::vector<int> 	_admins;
	std::vector<int>	_invited;
	int					_userLimit;
	bool				_invitMode;
	bool				_restrictedTopic;
public:
	Channels();
	Channels(std::string name, int fdCreator);
	~Channels();
	void sendMessage(std::string message, int sender);
	void addAdmin(int userFD);
	bool VerifAdmin(int userFd);
	bool VerifUser(int userFd);
	bool VerifInvited(const int user);
	int addUser(const int user);
	void addInvited(const int user);
	void removeInvited(const int user);
	void removeAdmin(const int user);
	void deleteUser(const int user);
	std::vector<int> getUsers() const;
	std::vector<int> getAdmins() const;
	std::string getTopic() const;
	std::string getPassword() const;
	int	getUserLimit();
	bool getRestrictedTopic();
	bool getInvitMode();
	void setTopic(const std::string newTopic, int user);
	void setUserLimit(const int limit, int user);
	void setChannelPass(const std::string password, int user);
	void setInvitationMode(const std::string option, int user);
	void setRestrictedTopic(const std::string option, int user, std::string nickName);
};
