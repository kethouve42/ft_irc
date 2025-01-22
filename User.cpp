/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acasanov <acasanov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:55:21 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/22 18:31:53 by acasanov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User()
{
	is_user = true;
}

User::User(int fd)
{
	std::ostringstream oss;
	_userFd = fd;
	oss << fd;
	_nickName = oss.str();
	is_user = true;
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

std::string	User::getUserRealname() const
{
	return (this->_Realname);
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

void User::setUserRealname(const std::string Realname)
{
	this->_Realname = Realname;
}
