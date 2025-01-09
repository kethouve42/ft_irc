# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kethouve <kethouve@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/25 16:26:56 by kethouve          #+#    #+#              #
#    Updated: 2025/01/09 15:54:47 by kethouve         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
SRC = main.cpp Server.cpp Channels.cpp User.cpp
OBJ = $(SRC:.cpp=.o)
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g
CXX = c++
RM = rm -f

all: $(NAME)

$(NAME): $(OBJ)
		$(CXX) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
		$(RM) $(OBJ)

fclean: clean
		$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re