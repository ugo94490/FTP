##
## EPITECH PROJECT, 2020
## OOP_nanotekspice_2019
## File description:
## Makefile
##

SRC	=		main.c				\
			src/tab_tools.c		\
			src/word_tab.c		\
			src/free_word.c 	\
			src/client.c		\
			src/command.c 		\
			src/command_2.c 	\
			src/command_3.c 	\
			src/retr.c			\
			src/tools.c 		\
			src/list.c 			\
			src/stor.c 			\
			src/init_server.c	\

SRC_T	=	tests/test.c 		\
			src/tab_tools.c		\
			src/word_tab.c		\
			src/free_word.c 	\
			src/tools.c 		\
			src/command_2.c 	\
			src/command_3.c 	\

OBJ	=	$(SRC:.c=.o)

TESTS_NAME	=	unit_tests

TEST_FLAGS	=	-lcriterion --coverage

NAME	=	myftp

CFLAGS =	-Wall -Wextra

CPPFLAGS	=	-I./include

.DEFAULT := all

all: $(NAME)

$(NAME):	$(OBJ)
	gcc -o $(NAME) $(OBJ) $(CPPFLAGS) $(CFLAGS)

tests_run 	:
			gcc $(SRC_T) $(CFLAGS) $(CPPFLAGS) $(TEST_FLAGS) -o $(TESTS_NAME)
			./unit_tests
			gcovr -r . --exclude tests
			gcovr -r . -b --exclude tests

tests_clean 	:
			rm *.gcda
			rm *.gcno
			rm unit_tests

clean:
	rm -f $(NAME)

fclean:	clean
	rm -f $(OBJ)

re: fclean all

.PHONY: all clean fclean re tests_run tests_clean
