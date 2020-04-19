/*
** EPITECH PROJECT, 2020
** test.c
** File description:
** test.c
*/

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ftp.h"

Test(ftp, tab_len)
{
    char **tab = malloc(sizeof(char *) * 2);
    int len = 0;

    tab[0] = strdup("test");
    tab[1] = NULL;
    len = my_strlen_tab(tab);
    cr_assert_eq(len, 1);
}

Test(ftp, tab_len_err)
{
    char **tab = NULL;
    int len = 0;

    len = my_strlen_tab(tab);
    cr_assert_eq(len, 0);
}

Test(ftp, create_tab)
{
    char **tab = word_tab("A B C D E F", " ");
    int len = 0;

    len = my_strlen_tab(tab);
    cr_assert_eq(len, 6);
}

Test(ftp, create_tab_err)
{
    char **tab = word_tab("A B C D E", "");

    cr_assert_eq(tab, NULL);
}

Test(ftp, free_tab_fct)
{
    char **tab = word_tab("A B C D E F", " ");

    my_free_tab(tab);
    tab = NULL;
    cr_assert_eq(tab, NULL);
}

Test(ftp, array_copy)
{
    char **tab = word_tab("A B C D E F", " ");
    char **copy = my_arraycpy(tab);
    int len = my_strlen_tab(copy);

    cr_assert_eq(len, 6);
}

Test(ftp, print_tab)
{
    char **tab = word_tab("A B C D E F", " ");
    int ret = print_array(tab);

    cr_assert_eq(ret, 0);
}

Test(ftp, print_tab_err)
{
    char **tab = NULL;
    int ret = print_array(tab);

    cr_assert_eq(ret, 84);
}

Test(ftp, parse_port_int)
{
    int ip = parse_port("(127,0,0,1,13,56)");
    int verif = 13 * 256 + 56;

    cr_assert_eq(ip, verif);
}

Test(ftp, parse_port_err)
{
    int ip = parse_port("(127,0,0,1,a,56)");
    int verif = -1;

    cr_assert_eq(ip, verif);
}

Test(ftp, parse_port_err_branch)
{
    int ip = parse_port("(127,0,0,1,13,a)");
    int verif = -1;

    cr_assert_eq(ip, verif);
}

Test(ftp, init_addr)
{
    struct sockaddr_in test = init_my_addr(12450);

    cr_assert_neq(&test, NULL);
}

Test(ftp, dupcat_str)
{
    char *str = strdup("abc");
    char *str1 = strdup("abc");
    char *res = dupcat(str, str1, 3);

    cr_assert_neq(res, NULL);
}

Test(ftp, dupcat_without_str)
{
    char *str = NULL;
    char *str1 = strdup("abc");
    char *res = dupcat(str, str1, 3);

    cr_assert_neq(res, NULL);
}

Test(ftp, check_client_log)
{
    client_t *client = malloc(sizeof(client_t));
    int ret = 0;

    client->log = 0;
    client->fd = 0;
    ret = check_client(client);
    cr_assert_eq(ret, 84);
}

Test(ftp, check_client_mode)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 1;
    other->mode = -1;
    other->fd = 0;
    ret = check_client(other);
    cr_assert_eq(ret, 84);
}

Test(ftp, check_client_without_err)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 1;
    other->mode = 0;
    other->fd = 0;
    ret = check_client(other);
    cr_assert_eq(ret, 0);
}

Test(ftp, pwd_err)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 0;
    other->fd = 0;
    ret = pwd(other);
    cr_assert_eq(ret, 0);
}

Test(ftp, pwd_no_err)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 1;
    other->fd = 0;
    ret = pwd(other);
    cr_assert_eq(ret, 0);
}

Test(ftp, quit_test)
{
    client_t *other = malloc(sizeof(client_t));
    int fd = open("tests/test.c", O_APPEND);
    int ret = 0;

    other->log = 1;
    other->fd = fd;
    ret = quit(other);
    cr_assert_eq(ret, 1);
}

Test(ftp, cdup_msg)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->fd = 0;
    cdup_message(other, ret);
    cr_assert_neq(other, NULL);
}

Test(ftp, cdup_msg_err)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 1;

    other->fd = 0;
    cdup_message(other, ret);
    cr_assert_neq(other, NULL);
}

Test(ftp, cdup_not_log)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 0;
    other->fd = 0;
    ret = cdup(other);
    cr_assert_eq(ret, 0);
}

Test(ftp, cdup_log)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 1;
    other->fd = 0;
    ret = cdup(other);
    cr_assert_eq(ret, 0);
}

Test(ftp, dele_not_log)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 0;
    other->fd = 0;
    ret = dele(other);
    cr_assert_eq(ret, 0);
}

Test(ftp, dele_log_err)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 0;
    other->fd = 0;
    other->command = word_tab("ABC", " ");
    ret = dele(other);
    cr_assert_eq(ret, 0);
}

Test(ftp, dele_log_err_three)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 1;
    other->fd = 0;
    other->command = word_tab("A B C", " ");
    ret = dele(other);
    cr_assert_eq(ret, 0);
}

Test(ftp, dele_err)
{
    client_t *other = malloc(sizeof(client_t));
    int ret = 0;

    other->log = 1;
    other->fd = 0;
    other->command = word_tab("A B", " ");
    ret = dele(other);
    cr_assert_eq(ret, 0);
}

Test(ftp, dele_ok)
{
    client_t *other = malloc(sizeof(client_t));
    int fd = open("test", O_CREAT | S_IRWXO);
    int ret = 0;

    close(fd);
    other->log = 1;
    other->fd = 0;
    other->command = word_tab("DELE test", " ");
    ret = dele(other);
    cr_assert_eq(ret, 0);
}