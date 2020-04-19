/*
** EPITECH PROJECT, 2020
** tools.c
** File description:
** tools.c
*/

#include "ftp.h"

struct sockaddr_in init_my_addr(int port)
{
    struct sockaddr_in my_addr;

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(port);
    return (my_addr);
}

char *dupcat(char *str, char *str1, int n)
{
    char *new;

    if (str) {
        new = malloc(sizeof(char) * (strlen(str) + n + 1));
        new = strcpy(new, str);
        new = strcat(new, str1);
        free(str);
    } else {
        new = malloc(sizeof(char) * (n + 1));
        new = strncpy(new, str1, n);
        new[n] = '\0';
    }
    return (new);
}

int str_digit(char *str)
{
    for (int i = 0; str[i]; i++)
        if (isdigit(str[i]) == 0)
            return (1);
    return (0);
}

int check_client(client_t *client)
{
    if (client->log != 1) {
        dprintf(client->fd, "530 Not Connected.\r\n");
        return (84);
    }
    if (client->mode == -1) {
        dprintf(client->fd, "425 Can't open data connection.\r\n");
        return (84);
    }
    return (0);
}

int parse_port(char *ip)
{
    char *str = NULL;
    char **tab = NULL;
    int res = 0;

    if (ip[0] == '(')
        str = strdup(ip + 1);
    if (str[strlen(str) - 1] == ')')
        str[strlen(str) - 1] = 0;
    tab = word_tab(str, ",");
    if (str_digit(tab[4]) == 1 || str_digit(tab[5]) == 1)
        return (-1);
    res = atoi(tab[4]) * 256 + atoi(tab[5]);
    my_free_tab(tab);
    free(str);
    return (res);
}