/*
** EPITECH PROJECT, 2020
** main.c
** File description:
** main.c
*/

#include "ftp.h"

int main(int ac, char **av)
{
    int my_socket = 0;
    fd_set current;
    fd_set ready;

    if (ac == 2 && strcmp(av[1], "-help") == 0)
        return (usage(av[0]));
    if (ac != 3 || chdir(av[2]) != 0)
        return (84);
    srand(time(NULL));
    if ((my_socket = init_server(atoi(av[1]))) == 0)
        return (84);
    FD_ZERO(&current);
    FD_SET(my_socket, &current);

    while (1) {
        ready = current;
        if (select(FD_SETSIZE, &ready, NULL, NULL, NULL) < 0)
            return (0);
        current = check_connection(current, ready, my_socket, av);
    }
    return (0);
}
