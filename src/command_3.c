/*
** EPITECH PROJECT, 2020
** command_3.c
** File description:
** command_3.c
*/

#include "ftp.h"

int pasv(client_t *client)
{
    int port = 0;

    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else {
        if (client->mode != -1) {
            client->mode = -1;
            close(client->sock.fd);
        }
        client->sock.fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client->sock.fd == -1)
            return (0);
        port = rand() % 3000 + 1024;
        client->sock.my_addr = init_my_addr(port);
        init_socket_pasv(client);
        client->mode = 0;
        dprintf(client->fd, "227 Entering Passive Mode (127,0,0,1");
        dprintf(client->fd, ",%d,%d).\r\n", port / 256, port % 256);
    }
    return (0);
}

int port(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else if (my_strlen_tab(client->command) != 2)
        dprintf(client->fd, "501 Bad Argument.\r\n");
    else {
        if (client->mode != -1) {
            client->mode = -1;
            close(client->sock.fd);
        }
        port_check(client);
        dprintf(client->fd, "200 PORT okay.\r\n");
    }
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