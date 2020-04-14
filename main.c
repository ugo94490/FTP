/*
** EPITECH PROJECT, 2020
** main.c
** File description:
** main.c
*/

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include "ftp.h"

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

int user(client_t *client)
{
    if (my_strlen_tab(client->command) > 2) {
        dprintf(client->fd, "xxx USER Bad parameter.\n");
        return (0);
    }
    if (my_strlen_tab(client->command) < 2 && client->log == -1)
        dprintf(client->fd, "xxx Bad user.\n");
    else if (client->log == 1)
        dprintf(client->fd, "xxx Already Connected.\n");
    else {
        client->user = strdup(client->command[1]);
        client->log = 0;
        dprintf(client->fd, "331 User name okay, need password.\n");
    }
    return (0);
}

int pass(client_t *client)
{
    if (my_strlen_tab(client->command) > 2) {
        dprintf(client->fd, "xxx PASS Bad parameter.\n");
        return (0);
    }
    if (client->log == -1)
        dprintf(client->fd, "332 Need account for login.\n");
    else if (client->log == 1)
        dprintf(client->fd, "xxx Already Connected.\n");
    else {
        if (strncmp(client->user, "Anonymous", 9) == 0 && my_strlen_tab(client->command) == 1) {
            dprintf(client->fd, "230 User logged in, proceed.\n");
            client->log = 1;
        } else
            dprintf(client->fd, "xxx Bad login.\n");
    }
    return (0);
}

int cwd(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    (void)client;
    return (0);
}

int cdup(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    (void)client;
    return (0);
}

int quit(client_t *client)
{
    dprintf(client->fd, "221 Service closing control connection\n");
    return (1);
}

int dele(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    (void)client;
    return (0);
}

int pwd(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    (void)client;
    return (0);
}

int pasv(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    (void)client;
    return (0);
}

int port(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    (void)client;
    return (0);
}

int help(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    (void)client;
    return (0);
}

int noop(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    dprintf(client->fd, "200 NOOP okay.\n");
    return (0);
}

int retr(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    (void)client;
    return (0);
}

int stor(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "xxx Not Connected");
    (void)client;
    return (0);
}

char *dupcat(char *str, char *str1, int n)
{
    char *new;

    if (str) {
        new = malloc(sizeof(char) * (strlen(str) + n + 1));
        new = strcpy(new, str);
        new = strcat(new, str1);
        free(str);
    } else {
        new = malloc(sizeof(char) * (n + 1)); 
        new = strncpy(new, str1, n);
        new[n] = '\0';
    }
    return (new);
}

int list(client_t *client)
{
    pid_t pid = 0;
    int link[2];
    char tmp[4096];
    int n = 0;
    char *res = NULL;

    if (client->log != 1) {
        dprintf(client->fd, "xxx Not Connected");
        return (0);
    }
    if (pipe(link) == -1) {
        perror("PIPE");
        return (0);
    }
    if ((pid = fork()) == -1) {
        perror("FORK LS");
        return (0);
    }
    if (pid == 0) {
        dup2(link[1], STDOUT_FILENO);
        close(link[0]);
        close (link[1]);
        execl("/bin/ls", "ls", "-l", (char *)0);
    } else {
        while ((n = read(link[0], tmp, sizeof(tmp))) > 0) {
            res = dupcat(res, tmp, n);
            if (n < 4096)
                break;
        }
        wait(NULL);
    }
    dprintf(client->fd, "150 File status okay; about to open data connection.\n");
    dprintf(client->fd, "%s", res);
    dprintf(client->fd, "226 Closing data connection.\n");
    free(res);
    return (0);
}

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

int usage(char *str)
{
    printf("USAGE: %s port path\n", str);
    printf("       port  is the port number on which the server socket listens\n");
    printf("       path  is the path to the home directory for the Anonymous");
    printf(" user\n");
    return (0);
}

int init_server(void)
{
    int my_socket = 0;
    int option = 1;
    struct sockaddr_in my_addr;
    socklen_t lenght_socket = sizeof(my_addr);

    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (my_socket == -1) {
        perror("Socket");
        return (0);
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(4242);
    lenght_socket = sizeof(my_addr);
    if (setsockopt(my_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0) {
        perror("Setsockopt");
        return (0);
    }
    if (bind(my_socket, (struct sockaddr *) &my_addr, lenght_socket) == -1) {
        perror("Bind");
        return (0);
    }
    if (listen(my_socket, 3) == -1)
        perror("Listen");
    return (my_socket);
}

int choose_command(client_t *client)
{
    for (int i = 0; i < 14; i++) {
        if (strncmp(client->command[0], cmd[i], strlen(cmd[i]) - 1) == 0) {
            return (ptr[i](client));
        }
    }
    dprintf(client->fd, "500 Unknow command.\n");
    return (0);
}

int free_client(client_t *client)
{
    my_free_tab(client->env);
    my_free_tab(client->command);
    free(client);
    return (0);
}

int exec_client_connection(int fd, char **env)
{
    FILE *stream = fdopen(fd, "r");
    char *str = NULL;
    size_t n;
    client_t *client = malloc(sizeof(client_t));

    client->fd = fd;
    client->log = -1;
    client->env = my_arraycpy(env);
    dprintf(client->fd, "220 Service ready for new user.\n");
    while (getline(&str, &n, stream) >= 0) {
        client->command = word_tab(str, " ");
        if (choose_command(client) == 1)
            break;
        str = NULL;
    }
    free_client(client);
    fclose(stream);
    return (0);
}

int accept_client(int my_socket, struct sockaddr_in my_addr)
{
    socklen_t lenght_socket = sizeof(my_addr);
    int socket_fd = accept(my_socket, (struct sockaddr *) &my_addr, &lenght_socket);

    if (socket_fd == -1)
        printf("error accept");
    return socket_fd;
}

struct sockaddr_in init_my_addr(void)
{
    struct sockaddr_in my_addr;

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(4242);
    return (my_addr);
}

int main(int ac, char **av, char **env)
{
    int my_socket = 0;
    int client_socket = 0;
    struct sockaddr_in my_addr = init_my_addr();
    fd_set current;
    fd_set ready;
    pid_t pid = 0;

    if (ac == 2 && strcmp(av[1], "-help") == 0)
        return (usage(av[0]));
    if ((my_socket = init_server()) == 0)
        return (84);
    FD_ZERO(&current);
    FD_SET(my_socket, &current);

    //printf("%d\n", my_socket);
    //printf("%d\n", FD_SETSIZE);
    while (1) {
        ready = current;
        if (select(FD_SETSIZE, &ready, NULL, NULL, NULL) < 0) {
            perror("SELECT");
            return (0);
        }
        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &ready)) {
                if (i == my_socket) {
                    client_socket = accept_client(my_socket, my_addr);
                    FD_SET(client_socket, &current);
                } else {
                    if ((pid = fork()) == -1) {
                        perror("FORK CLIENT");
                        return (0);
                    }
                    if (pid == 0)
                        exec_client_connection(i, env);
                    FD_CLR(i, &current);
                }
            }
        }
    }
    return (0);
}
