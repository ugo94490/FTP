##
## EPITECH PROJECT, 2020
## OOP_nanotekspice_2019
## File description:
## Makefile
##

SRC	=		main.c		\
			tab_tools.c	\
			word_tab.c	\
			free_word.c \

SRC_T	=

OBJ	=	$(SRC:.c=.o)

TESTS_NAME	=	unit_tests

TEST_FLAGS	=	-lcriterion --coverage

NAME	=	myftp

CFLAGS =	-Wall -Wextra -g3

CPPFLAGS	=	-I./include -g3

.DEFAULT := all

all: $(NAME)

$(NAME):	$(OBJ)
	gcc -o $(NAME) $(OBJ) $(CPPFLAGS) $(CFLAGS) -g3

tests_run 	:
			gcc $(SRC_T) $(CFLAGS) $(CPPFLAGS) $(TEST_FLAGS) -o $(TESTS_NAME)
			./unit_tests
			gcovr -r . --exclude tests
			gcovr -r . -b --exclude tests

tests_clean 	:
			rm tests/*.gcda
			rm tests/*.gcno
			rm *.gc*
			rm unit_tests

clean:
	rm -f $(NAME)

fclean:	clean
	rm -f $(OBJ)

re: fclean all

.PHONY: all clean fclean re tests_run tests_clean
