/*
** EPITECH PROJECT, 2020
** command.c
** File description:
** command.c
*/

#include "ftp.h"

int user(client_t *client)
{
    if (my_strlen_tab(client->command) > 2) {
        dprintf(client->fd, "501 Too many parameters for USER.\r\n");
        return (0);
    }
    if (my_strlen_tab(client->command) < 2 && client->log == -1)
        dprintf(client->fd, "501 Not Enough argument USER.\r\n");
    else if (client->log == 1)
        dprintf(client->fd, "530 Already Connected.\r\n");
    else {
        client->user = strdup(client->command[1]);
        client->log = 0;
        dprintf(client->fd, "331 %s okay,", client->command[1]);
        dprintf(client->fd, " need password.\r\n");
    }
    return (0);
}

int pass(client_t *client)
{
    if (my_strlen_tab(client->command) > 2) {
        dprintf(client->fd, "501 Too many parameters for PASS.\r\n");
        return (0);
    }
    if (client->log == -1)
        dprintf(client->fd, "332 Need account for login.\r\n");
    else if (client->log == 1)
        dprintf(client->fd, "530 Already Connected.\r\n");
    else {
        if (strncmp(client->user, "Anonymous", 9) == 0) {
            dprintf(client->fd, "230 User logged in, proceed.\r\n");
            client->log = 1;
        } else
            dprintf(client->fd, "530 Not Logged in.\r\n");
    }
    return (0);
}

int check_chdir(client_t *client)
{
    int ret = 0;
    char *resolved = NULL;

    if (client->command[1][0] != '/') {
        resolved = realpath(client->command[1], resolved);
        if (resolved != NULL) {
            ret = chdir(client->command[1]);
            free(resolved);
        }
        if (resolved == NULL)
            ret = -1;
    } else {
        if (strlen(client->command[1]) > strlen(client->path))
            ret = chdir(client->command[1]);
        else
            ret = 0;
    }
    return (ret);
}

int cwd(client_t *client)
{
    int ret = 0;

    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else if (my_strlen_tab(client->command) >= 3)
        dprintf(client->fd, "550 Failed to change directory.\r\n");
    else {
        if (my_strlen_tab(client->command) == 1)
            ret = 1;
        else
            ret = check_chdir(client);
        if (ret == 0) {
            dprintf(client->fd, "250 Requested file action okay,");
            dprintf(client->fd, " completed.\r\n");
        } else
            dprintf(client->fd, "550 Failed to change directory.\r\n");
    }
    return (0);
}

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