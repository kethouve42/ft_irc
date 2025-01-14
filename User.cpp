/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:55:21 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/14 15:37:42 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User()
{}

User::User(int fd)
{
	_userFd = fd;
	_nickName = std::to_string(fd);
	_username = nullptr;
}

User::~User(){}

/*Getter*/
int	User::getUserFd() const
{
	return (this->_userFd);
}

std::string	User::getUserNickName() const
{
	return (this->_nickName);
}

/*Setter*/

void User::setUserNickName(const std::string name)
{
	this->_nickName = name;
}

void User::setUserName(const std::string username)
{
	this->_username = username;
}
