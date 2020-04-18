/*
** EPITECH PROJECT, 2019
** word_tab.c
** File description:
** word_tab.c
*/

#include <stdlib.h>

char **free_tab(char const *src, size_t nb_line, char **tab)
{
    free((char *)src);
    *(tab + nb_line) = NULL;
    return (tab);
}
