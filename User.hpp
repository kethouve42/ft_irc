/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:29:56 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/09 15:52:22 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"

class User
{
private:
	int _userFd;
	std::string _nickName;
public:
	User();
	~User();

	/*Getter*/
	int	getUserFd();
	std::string	getUserNickName();
	/*Setter*/
	void setUserFd(int fd);
	void setUserNickName(std::string name);
};
