/*
** EPITECH PROJECT, 2020
** ftp.h
** File description:
** ftp.h
*/

#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#include <limits.h>
#include <time.h>
#include <ctype.h>

typedef struct sock_s
{
    int fd;
    struct sockaddr_in my_addr;
    int fd_client;
} sock_t;

typedef struct client_s
{
    int log;
    int fd;
    char **command;
    char *user;
    int mode;
    char *path;
    sock_t sock;
} client_t;

int my_strlen_tab(char **tab);
int my_free_tab(char **tab);
char **my_arraycpy(char **tab);
int print_array(char **tab);
char **word_tab(char const *src, char const *del);
char **free_tab(char const *src, size_t nb_line, char **tab);

/* CLIENT.C */
int exec_client_connection(int fd, char *path);
int free_client(client_t *client);
int choose_command(client_t *client);

/* COMMAND.C */
int user(client_t *client);
int pass(client_t *client);
int check_chdir(client_t *client);
int cwd(client_t *client);

/* COMMAND_2.C */
void cdup_message(client_t *client, int ret);
int cdup(client_t *client);
int quit(client_t *client);
int dele(client_t *client);
int pwd(client_t *client);

/* COMMAND_3.C */
int pasv(client_t *client);
int port(client_t *client);
int help(client_t *client);
int noop(client_t *client);
int usage(char *str);

/* RETR.C */
int retr(client_t *client);
void retr_exec(client_t *client, char *res);
int connection_abort(client_t *client);
int retr_connection(client_t *client);

/* TOOLS.C */
struct sockaddr_in init_my_addr(int port);
char *dupcat(char *str, char *str1, int n);
int str_digit(char *str);
int check_client(client_t *client);
int parse_port(char *ip);

/* LIST.C */
void list_data(client_t *client, char *res);
char *read_list(char *res, int link[2]);
void exec_list(int link[2], client_t *client);
int list(client_t *client);

/* STOR.C */
int port_check(client_t *client);
void stor_exec(client_t *client, socklen_t lenght_socket);
int stor_connection(client_t *client);
int stor(client_t *client);

/* INIT_SERVER.C */
fd_set check_connection(fd_set current, fd_set ready, int my_socket, char **av);
fd_set check_ready(int i, fd_set current, int my_socket, char **av);
int accept_client(int my_socket, struct sockaddr_in my_addr);
int init_server(int port);
int init_socket_pasv(client_t *client);