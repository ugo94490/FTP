/*
** EPITECH PROJECT, 2020
** stor.c
** File description:
** stor.c
*/

#include "ftp.h"

static const char *MSG_150 = "150 File status okay; about to open "
"data connection.\r\n";

static const char *MSG_451 = "451 Requested action aborted: local error "
"in processing.\r\n";

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

void stor_exec(client_t *client, struct stat buffer, char *name, char *content)
{
    socklen_t lenght_socket;
    FILE *file = NULL;
    FILE *stream = NULL;

    lenght_socket = sizeof(client->sock.my_addr);
    client->sock.fd_client = accept(client->sock.fd, (struct sockaddr *)
    &client->sock.my_addr, &lenght_socket);
    stream = fopen(client->command[1], "r");
    content = malloc(sizeof(char) * (buffer.st_size + 1));
    fread(content, buffer.st_size + 1, sizeof(char), stream);
    fclose(stream);
    content[buffer.st_size] = 0;
    file = fopen(name, "w+");
    fwrite(content, buffer.st_size, sizeof(char), file);
    fclose(file);
    dprintf(client->fd, "226 Closing data connection.\r\n");
    close(client->sock.fd);
    close(client->sock.fd_client);
    client->mode = -1;
}

char *get_name(client_t *client, char *name)
{
    int len = 0;
    int i = 0;
    int offset = 0;

    len = strlen(client->command[1]);
    for (; len != 0 && client->command[1][len] != '/'; len--, i++);
    name = malloc(sizeof(char) * (i + 1));
    for (int j = 0; client->command[1][j]; j++)
        if (client->command[1][j] == '/')
            offset = 1;
    name = strncpy(name, client->command[1] + len + offset, i);
    name[i] = '\0';
    return (name);
}

int stor_connection(client_t *client)
{
    struct stat buffer;
    char *name = NULL;
    pid_t pid = 0;
    char *content = NULL;

    if (stat(client->command[1], &buffer) != 0)
        return (dprintf(client->fd, MSG_451));
    name = get_name(client, name);
    dprintf(client->fd, MSG_150);
    pid = fork();
    if (pid == 0) {
        stor_exec(client, buffer, name, content);
        close(client->sock.fd);
        close(client->sock.fd_client);
        client->mode = -1;
        exit(0);
    }
    free(content);
    free(name);
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