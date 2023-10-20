# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: axfernan <axfernan@student.42nice.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/26 11:05:14 by axfernan          #+#    #+#              #
#    Updated: 2023/10/20 14:50:25 by axfernan         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

FLAGS = -Werror -Wextra -Wall -std=c++98

SRC = $(MAIN_SRC) $(MAIN_HDR)

MAIN = *.cpp
HMAIN = *.hpp

MAIN_SRC = $(addprefix srcs/, $(MAIN))
MAIN_HDR = $(addprefix includes/, $(HMAIN))

all: $(NAME)

OBJ = $(MAIN:.cpp=.o)

NONE='\033[0m'
GREEN='\033[32m'
YELLOW='\033[33m'
GRAY='\033[2;37m'
CURSIVE='\033[3m'

$(NAME) : $(OBJ) $(OBJ_BONUS)
	@echo $(CURSIVE)$(GRAY) "     - Compiling $(NAME)..." $(NONE)
	@c++  $(FLAGS) $(OBJ) -o $(NAME)
	@echo $(GREEN)"- Compiled -"$(NONE)

$(OBJ): $(SRC)
	@echo $(CURSIVE)$(GRAY) "     - Making $(NAME) object files..." $(NONE)
	@c++ $(FLAGS) -c $(SRC)

clean:
	@rm -f *.o
	@echo $(CURSIVE)$(YELLOW) "     - Removing $(NAME) objects files..." $(NONE)

fclean: clean
	@rm -f $(NAME)
	@echo $(CURSIVE)$(YELLOW) "     - Removing $(NAME) executable..." $(NONE)

re: fclean all

.PHONY: all clean fclean re