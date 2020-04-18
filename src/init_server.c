/*
** EPITECH PROJECT, 2020
** init_server.c
** File description:
** init_server.c
*/

#include "ftp.h"

int init_socket_pasv(client_t *client)
{
    uint16_t size = 0;
    int option = 1;
    size = sizeof(client->sock.my_addr);
    if (setsockopt(client->sock.fd, SOL_SOCKET, (SO_REUSEPORT |
        SO_REUSEADDR), (char *)&option, sizeof(option)) < 0)
        return (0);
    if (bind(client->sock.fd, (struct sockaddr *) &client->sock.my_addr,
        size) == -1)
        return (0);
    if (listen(client->sock.fd, 100) == -1)
        perror("Listen");
    return (0);
}

int init_server(int port)
{
    int my_socket = 0;
    int option = 1;
    struct sockaddr_in my_addr = init_my_addr(port);
    socklen_t lenght_socket = sizeof(my_addr);

    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (my_socket == -1)
        return (0);
    lenght_socket = sizeof(my_addr);
    if (setsockopt(my_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR),
        (char *)&option, sizeof(option)) < 0)
        return (0);
    if (bind(my_socket, (struct sockaddr *) &my_addr, lenght_socket) == -1)
        return (0);
    if (listen(my_socket, 100) == -1)
        perror("Listen");
    return (my_socket);
}

int accept_client(int my_socket, struct sockaddr_in my_addr)
{
    socklen_t lenght = sizeof(my_addr);
    int socket_fd = accept(my_socket, (struct sockaddr *) &my_addr, &lenght);

    if (socket_fd == -1)
        printf("error accept");
    else
        dprintf(socket_fd, "220 Service ready for new user.\r\n");
    return socket_fd;
}

fd_set check_ready(int i, fd_set current, int my_socket, char **av)
{
    int client_socket = 0;
    pid_t pid = 0;
    struct sockaddr_in my_addr = init_my_addr(atoi(av[1]));

    if (i == my_socket) {
        client_socket = accept_client(my_socket, my_addr);
        FD_SET(client_socket, &current);
    } else {
        if ((pid = fork()) == -1)
            return (current);
        if (pid == 0) {
            exec_client_connection(i, av[2]);
            exit(0);
        }
        close(i);
        FD_CLR(i, &current);
    }
    return (current);
}

fd_set check_connection(fd_set current, fd_set ready, int my_socket, char **av)
{
    for (int i = 0; i < FD_SETSIZE; i++)
        if (FD_ISSET(i, &ready))
            current = check_ready(i, current, my_socket, av);
    return (current);
}