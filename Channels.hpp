/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:27:28 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/15 16:10:44 by kethouve         ###   ########.fr       */
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
	std::vector<int> 	_user;
	std::vector<int> 	_admins;
	int					_userLimit;
public:
	Channels();
	Channels(std::string name, int fdCreator);
	~Channels();
	void sendMessage(std::string message, int sender);
	void addAdmin(int userFD);
	bool VerifAdmin(int userFd);
	bool VerifUser(int userFd);

	/*Getter*/
	std::vector<int> getUsers() const;
	/*Setter*/
	void addUser(const int user);
	void deleteUser(const int user);
	void setUserLimit(const int limit);
	void setChannelPass(const std::string password);
};
