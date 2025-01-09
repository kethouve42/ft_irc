/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 15:27:28 by kethouve          #+#    #+#             */
/*   Updated: 2025/01/09 15:47:52 by kethouve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"

class User;

class Channels
{
private:
	std::vector<int> _user;
	std::vector<int> _admins;
public:
	Channels();
	~Channels();
	void sendMessage(std::string message, int sender);

	/*Getter*/
	int getOperator();
	/*Setter*/
	void setUser(int user);
	void setAdmin(int admin);

};
