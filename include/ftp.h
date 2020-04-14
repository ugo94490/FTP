/*
** EPITECH PROJECT, 2020
** ftp.h
** File description:
** ftp.h
*/

#pragma once

typedef struct client_s
{
    int log;
    int fd;
    char **command;
    char **env;
    char *user;
} client_t;

int my_strlen_tab(char **tab);
int my_free_tab(char **tab);
char **my_arraycpy(char **tab);
int print_array(char **tab);
char **word_tab(char const *src, char const *del);
char **free_tab(char const *src, size_t nb_line, char **tab);