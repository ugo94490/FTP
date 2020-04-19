/*
** EPITECH PROJECT, 2020
** retr.c
** File description:
** retr.c
*/

#include "ftp.h"

static const char *MSG_150 = "150 File status okay; about to open "
"data connection.\r\n";

int retr(client_t *client)
{
    if (client->log != 1) {
        dprintf(client->fd, "530 Not Connected.\r\n");
        return (0);
    }
    if (client->mode == -1)
        dprintf(client->fd, "425 Can't open data connection.\r\n");
    else if (my_strlen_tab(client->command) != 2)
        dprintf(client->fd, "501 Too many parameters for PASS.\r\n");
    else
        retr_connection(client);
    return (0);
}

void retr_exec(client_t *client, char *res)
{
    socklen_t lenght_socket;
    lenght_socket = sizeof(client->sock.my_addr);
    client->sock.fd_client = accept(client->sock.fd, (struct sockaddr *)
    &client->sock.my_addr, &lenght_socket);
    dprintf(client->sock.fd_client, "%s", res);
    dprintf(client->fd, "226 Closing data connection.\r\n");
    close(client->sock.fd);
    close(client->sock.fd_client);
}

int connection_abort(client_t *client)
{
    dprintf(client->fd, "451 Requested action aborted: local error in");
    dprintf(client->fd, " processing.\r\n");
    return (0);
}

int retr_connection(client_t *client)
{
    pid_t pid = 0;
    struct stat buffer;
    FILE *stream;
    char *res = NULL;

    if (stat(client->command[1], &buffer) != 0)
        return (connection_abort(client));
    res = malloc(sizeof(char) * (buffer.st_size + 1));
    stream = fopen(client->command[1], "r");
    fread(res, buffer.st_size, 1, stream);
    res[buffer.st_size] = 0;
    dprintf(client->fd, MSG_150);
    pid = fork();
    if (pid == 0) {
        retr_exec(client, res);
        exit(0);
    }
    client->mode = -1;
    close(client->sock.fd);
    free(res);
    return (0);
}