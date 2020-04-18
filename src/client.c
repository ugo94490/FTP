/*
** EPITECH PROJECT, 2020
** client.c
** File description:
** client.c
*/

#include "ftp.h"

static int (*ptr[])(client_t *client) = {
    &user,
    &pass,
    &cwd,
    &cdup,
    &quit,
    &dele,
    &pwd,
    &pasv,
    &port,
    &help,
    &noop,
    &retr,
    &stor,
    &list
};

static const char *cmd[14] = {
    "USER",
    "PASS",
    "CWD",
    "CDUP",
    "QUIT",
    "DELE",
    "PWD",
    "PASV",
    "PORT",
    "HELP",
    "NOOP",
    "RETR",
    "STOR",
    "LIST"
};

int choose_command(client_t *client)
{
    for (int i = 0; i < 14; i++) {
        if (strncmp(client->command[0], cmd[i], strlen(cmd[i])) == 0) {
            return (ptr[i](client));
        }
    }
    dprintf(client->fd, "500 Unknow command.\r\n");
    return (0);
}

int free_client(client_t *client)
{
    my_free_tab(client->command);
    free(client->path);
    free(client);
    return (0);
}

int exec_client_connection(int fd, char *path)
{
    FILE *stream = fdopen(fd, "r");
    char *str = NULL;
    size_t n;
    client_t *client = malloc(sizeof(client_t));

    client->fd = fd;
    client->log = -1;
    client->mode = -1;
    client->path = strdup(path);
    while (getline(&str, &n, stream) >= 0) {
        (str[strlen(str) - 1] == '\n') ? str[strlen(str) - 1] = '\0' : 0;
        (str[strlen(str) - 1] == '\r') ? str[strlen(str) - 1] = '\0' : 0;
        client->command = word_tab(str, " ");
        if (choose_command(client) == 1)
            break;
        str = NULL;
    }
    free_client(client);
    fclose(stream);
    return (0);
}