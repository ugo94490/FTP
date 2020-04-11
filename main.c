/*
** EPITECH PROJECT, 2020
** main.c
** File description:
** main.c
*/

#include <stdio.h>
#include <string.h>

int help(char *str)
{
    printf("USAGE: %s port path\n", str);
    printf("       port  is the port number on which the server socket listens\n");
    printf("       path  is the path to the home directory for the Anonymous");
    printf(" user\n");
    return (0);
}

int main(int ac, char **av)
{
    if (ac == 2 && strcmp(av[1], "-help") == 0)
        return (help(av[0]));
    return (0);
}
