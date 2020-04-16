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

typedef struct sock_s
{
    int fd;
    struct sockaddr_in my_addr;
} sock_t;

typedef struct client_s
{
    int log;
    int fd;
    char **command;
    char **env;
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