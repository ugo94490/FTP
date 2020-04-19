/*
** EPITECH PROJECT, 2020
** command_3.c
** File description:
** command_3.c
*/

#include "ftp.h"

int connection_abort(client_t *client)
{
    dprintf(client->fd, "451 Requested action aborted: local error in");
    dprintf(client->fd, " processing.\r\n");
    return (0);
}

int help(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else {
        dprintf(client->fd, "214 Command available: USER, PASS, CWD, CDUP");
        dprintf(client->fd, " QUIT, DELE, PWD, PASV, PORT, HELP, NOOP, RETR");
        dprintf(client->fd, " STOR, LIST.\r\n");
    }
    return (0);
}

int noop(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    dprintf(client->fd, "200 NOOP okay.\r\n");
    return (0);
}

int usage(char *str)
{
    printf("USAGE: %s port path\n", str);
    printf("       port  is the port number on which the server socket");
    printf(" listens\n");
    printf("       path  is the path to the home directory for the Anonymous");
    printf(" user\n");
    return (0);
}