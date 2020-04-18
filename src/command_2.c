/*
** EPITECH PROJECT, 2020
** command_2.c
** File description:
** command_2.c
*/

#include "ftp.h"

void cdup_message(client_t *client, int ret)
{
    if (ret == 0)
        dprintf(client->fd, "200 CDUP Okay.\r\n");
    else
        dprintf(client->fd, "550 Requested action not taken.\r\n");
}

int cdup(client_t *client)
{
    char path[256] = {0};
    int len = 0;
    char *dir = NULL;
    int ret = 0;

    if (client->log != 1) {
        dprintf(client->fd, "530 Not Connected.\r\n");
        return (0);
    }
    getcwd(path, sizeof(path));
    len = strlen(path);
    for (; len != 0 && path[len] != '/'; len--);
    dir = malloc(sizeof(char) * (len + 1));
    dir = strncpy(dir, path, len);
    dir[len] = '\0';
    ret = (strlen(dir) < strlen(client->path)) ? chdir(path) : chdir(dir);
    cdup_message(client, ret);
    free(dir);
    return (0);
}

int quit(client_t *client)
{
    dprintf(client->fd, "221 Service closing control connection.\r\n");
    close(client->fd);
    return (1);
}

int dele(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else if (my_strlen_tab(client->command) == 1 ||
        my_strlen_tab(client->command) > 2)
        dprintf(client->fd, "501 Bad Argument.\r\n");
    else {
        if (remove(client->command[1]) == 0) {
            dprintf(client->fd, "250 Requested file action okay,");
            dprintf(client->fd, " completed.\r\n");
        } else
            dprintf(client->fd, "550 Can't Delete file.\r\n");
    }
    return (0);
}

int pwd(client_t *client)
{
    char path[256];

    if (client->log != 1) {
        dprintf(client->fd, "530 Not Connected.\r\n");
        return (0);
    }
    getcwd(path, sizeof(path));
    dprintf(client->fd, "257 \"%s\" created.\r\n", path);
    return (0);
}