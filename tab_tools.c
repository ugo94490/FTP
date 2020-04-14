/*
** EPITECH PROJECT, 2019
** tab_tools.c
** File description:
** tab_tools.c
*/

#include <stdlib.h>

int my_strlen_tab(char **tab)
{
    int i = 0;

    if (tab == NULL)
        return (i);
    while (tab[i] != NULL)
        i++;
    return (i);
}

int my_free_tab(char **tab)
{
    for (int i = 0; i < my_strlen_tab(tab); i++)
        free(tab[i]);
    free(tab);
    return (0);
}

char **my_arraycpy(char **tab)
{
    char **copy = malloc(sizeof(char *) * (my_strlen_tab(tab) + 1));

    for (int i = 0; i < my_strlen_tab(tab); i++)
        copy[i] = strdup(tab[i]);
    copy[my_strlen_tab(tab)] = NULL;
    return (copy);
}

int print_array(char **tab)
{
    if (tab == NULL)
        return (84);
    for (int i = 0; i < my_strlen_tab(tab); i++)
        printf(tab[i]);
    return (0);
}
