/*
** EPITECH PROJECT, 2020
** stor.c
** File description:
** stor.c
*/

#include "ftp.h"

int port_check(client_t *client)
{
    uint16_t size = 0;
    int port = parse_port(client->command[1]);
    int option = 1;

    client->sock.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sock.fd == -1)
        return (0);
    client->sock.my_addr.sin_family = AF_INET;
    client->sock.my_addr.sin_addr.s_addr = INADDR_ANY;
    client->sock.my_addr.sin_port = htons(port);
    size = sizeof(client->sock.my_addr);
    if (setsockopt(client->sock.fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR),
        (char *)&option, sizeof(option)) < 0)
        return (0);
    if (bind(client->sock.fd, (struct sockaddr *) &client->sock.my_addr,
        size) == -1)
        return (0);
    if (listen(client->sock.fd, 100) == -1)
        return (0);
    client->mode = 1;
    return (0);
}

void stor_exec(client_t *client, socklen_t lenght_socket)
{
    FILE *stream = NULL;
    char tmp[4096];
    char *res = NULL;
    int n = 0;

    lenght_socket = sizeof(client->sock.my_addr);
    client->sock.fd_client = accept(client->sock.fd, (struct sockaddr *)
    &client->sock.my_addr, &lenght_socket);
    stream = fopen(client->command[1], "w+");
    while ((n = read(client->sock.fd_client, tmp, 4096)) > 0) {
        res = dupcat(res, tmp, n);
        if (n < 4096) {
            printf("%d\n", n);
            break;
        }
    }
    fwrite(res, strlen(res) + 1, sizeof(char), stream);
    fclose(stream);
    dprintf(client->fd, "226 Closing data connection.\r\n");
}

int stor_connection(client_t *client)
{
    pid_t pid = 0;
    socklen_t lenght_socket = 0;

    dprintf(client->fd, "150 File status okay; about to open data");
    dprintf(client->fd, " connection.\r\n");
    pid = fork();
    if (pid == 0) {
        stor_exec(client, lenght_socket);
        close(client->sock.fd);
        close(client->sock.fd_client);
        client->mode = -1;
        exit(0);
    }
    client->mode = -1;
    close(client->sock.fd);
    return (0);
}

int stor(client_t *client)
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
        stor_connection(client);
    return (0);
}