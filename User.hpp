/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:29:56 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/14 15:37:10 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"

class User
{
private:
	int 		_userFd;
	std::string _nickName;
	std::string _username;
	std::string _Realname;
public:
	bool is_user;
	bool user;
	bool nick;
	bool pass;
	User();
	User(int fd);
	~User();

	/*Getter*/
	int	getUserFd() const;
	std::string	getUserNickName() const;
	std::string	getUserRealname() const;
	/*Setter*/
	void setUserNickName(const std::string name);
	void setUserName(const std::string username);
	void setUserRealname(const std::string username);
};
