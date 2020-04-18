/*
** EPITECH PROJECT, 2020
** list.c
** File description:
** list.c
*/

#include "ftp.h"

static const char *MSG_150 = "150 File status okay; about to open "
"data connection.\r\n";

void list_data(client_t *client, char *res)
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

char *read_list(char *res, int link[2])
{
    int n = 0;
    char tmp[4096];

    while ((n = read(link[0], tmp, sizeof(tmp))) > 0) {
        res = dupcat(res, tmp, n);
        if (n < 4096)
            break;
    }
    return (res);
}

void exec_list(int link[2], client_t *client)
{
    dup2(link[1], STDOUT_FILENO);
    close(link[0]);
    close(link[1]);
    if (my_strlen_tab(client->command) == 1)
        execl("/bin/ls", "ls", "-l", (char *)0);
    if (my_strlen_tab(client->command) == 2)
        execl("/bin/ls", "ls", "-l", client->command[1], (char *)0);
}

int list(client_t *client)
{
    pid_t pid = 0;
    int link[2];
    char *res = NULL;

    if (check_client(client) == 84 || pipe(link) == -1 || (pid = fork()) == -1)
        return (0);
    if (pid == 0)
        exec_list(link, client);
    else
        res = read_list(res, link);
    wait(NULL);
    dprintf(client->fd, MSG_150);
    pid = fork();
    if (pid == 0) {
        list_data(client, res);
        exit(0);
    }
    client->mode = -1;
    close(client->sock.fd);
    free(res);
    return (0);
}