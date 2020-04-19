/*
** EPITECH PROJECT, 2019
** word_tab.c
** File description:
** word_tab.c
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ftp.h>

static bool end_word(char const c, char const *del)
{
    for (; *del; del += 1) {
        if (c == *del)
            return true;
    }
    return false;
}

static size_t word_len(char const *str, char const *del)
{
    size_t len = 0;

    for (len = 0; *(str + len) && !end_word(*(str + len), del); len += 1);
    return len;
}

static size_t get_nb_line(char const *str, char const *del)
{
    size_t i = 0;

    if (!str)
        return 0;
    for (; *(str + i); end_word(*(str + i), del) ? i += 1 : (size_t)str++);
    return ++i;
}

static char *findex(char **str, char const *del, size_t word_len)
{
    size_t i = 0;
    char *index = malloc(sizeof(char) * (word_len + 1));

    if (!index)
        return NULL;
    for (i = 0; !end_word(**str, del) && **str; i += 1)
        *(index + i) = *((*str)++);
    *(index + i) = '\0';
    for (; end_word(**str, del) && **str; (*str)++);
    return index;
}

char **word_tab(char const *src, char const *del)
{
    char *str = strdup(src);
    size_t nb_line = get_nb_line(str, del);
    size_t wlen = 0;
    char **tab = NULL;

    if (!nb_line || !del || !(*del) || !str)
        return NULL;
    tab = malloc(sizeof(char *) * (nb_line + 1));
    if (!tab)
        return NULL;
    src = str;
    for (size_t i = 0; i < nb_line; i += 1) {
        wlen = word_len(str, del);
        if ((!wlen && !(tab + i)) || !(*(tab + i) = findex(&str, del, wlen))) {
            free(tab);
            return NULL;
        }
    }
    return free_tab(src, nb_line, tab);
}