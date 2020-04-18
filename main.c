/*
** EPITECH PROJECT, 2020
** main.c
** File description:
** main.c
*/

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

struct sockaddr_in init_my_addr(int port)
{
    struct sockaddr_in my_addr;

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(port);
    return (my_addr);
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

int user(client_t *client)
{
    if (my_strlen_tab(client->command) > 2) {
        dprintf(client->fd, "501 Too many parameters for USER.\r\n");
        return (0);
    }
    if (my_strlen_tab(client->command) < 2 && client->log == -1)
        dprintf(client->fd, "501 Not Enough argument USER.\r\n");
    else if (client->log == 1)
        dprintf(client->fd, "530 Already Connected.\r\n");
    else {
        client->user = strdup(client->command[1]);
        client->log = 0;
        dprintf(client->fd, "331 %s okay,", client->command[1]);
        dprintf(client->fd, " need password.\r\n");
    }
    return (0);
}

int pass(client_t *client)
{
    if (my_strlen_tab(client->command) > 2) {
        dprintf(client->fd, "501 Too many parameters for PASS.\r\n");
        return (0);
    }
    if (client->log == -1)
        dprintf(client->fd, "332 Need account for login.\r\n");
    else if (client->log == 1)
        dprintf(client->fd, "530 Already Connected.\r\n");
    else {
        if (strncmp(client->user, "Anonymous", 9) == 0) {
            dprintf(client->fd, "230 User logged in, proceed.\r\n");
            client->log = 1;
        } else
            dprintf(client->fd, "530 Not Logged in.\r\n");
    }
    return (0);
}

int check_chdir(client_t *client)
{
    int ret = 0;
    char *resolved = NULL;

    if (client->command[1][0] != '/') {
        resolved = realpath(client->command[1], resolved);
        if (resolved != NULL && strlen(resolved) > strlen(client->path)) {
            ret = chdir(client->command[1]);
            free(resolved);
        }
        if (resolved == NULL)
            ret = -1;
    } else {
        if (strlen(client->command[1]) > strlen(client->path))
            ret = chdir(client->command[1]);
        else
            ret = 0;
    }
    return (ret);
}

int cwd(client_t *client)
{
    int ret = 0;

    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else if (my_strlen_tab(client->command) >= 3)
        dprintf(client->fd, "550 Failed to change directory.\r\n");
    else {
        if (my_strlen_tab(client->command) == 1)
            ret = chdir(client->path);
        else
            ret = check_chdir(client);
        if (ret == 0) {
            dprintf(client->fd, "250 Requested file action okay,");
            dprintf(client->fd, " completed.\r\n");
        } else
            dprintf(client->fd, "550 Failed to change directory.\r\n");
    }
    return (0);
}

void cdup_message(client_t *client, int ret)
{
    if (ret == 0)
        dprintf(client->fd, "200 CDUP Okay.\r\n");
    else
        dprintf(client->fd, "550 Requested action not taken.\r\n");
}

int cdup(client_t *client)
{
    char path[256] = {0};
    int len = 0;
    char *dir = NULL;
    int ret = 0;

    if (client->log != 1) {
        dprintf(client->fd, "530 Not Connected.\r\n");
        return (0);
    }
    getcwd(path, sizeof(path));
    len = strlen(path);
    for (; len != 0 && path[len] != '/'; len--);
    dir = malloc(sizeof(char) * (len + 1));
    dir = strncpy(dir, path, len);
    dir[len] = '\0';
    ret = (strlen(dir) < strlen(client->path)) ? chdir(path) : chdir(dir);
    cdup_message(client, ret);
    free(dir);
    return (0);
}

int quit(client_t *client)
{
    dprintf(client->fd, "221 Service closing control connection.\r\n");
    close(client->fd);
    return (1);
}

int dele(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else if (my_strlen_tab(client->command) == 1 ||
        my_strlen_tab(client->command) > 2)
        dprintf(client->fd, "501 Bad Argument.\r\n");
    else {
        if (remove(client->command[1]) == 0) {
            dprintf(client->fd, "250 Requested file action okay,");
            dprintf(client->fd, " completed.\r\n");
        } else
            dprintf(client->fd, "550 Can't Delete file.\r\n");
    }
    return (0);
}

int pwd(client_t *client)
{
    char path[256];

    if (client->log != 1) {
        dprintf(client->fd, "530 Not Connected.\r\n");
        return (0);
    }
    getcwd(path, sizeof(path));
    dprintf(client->fd, "257 \"%s\" created.\r\n", path);
    return (0);
}

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

int pasv(client_t *client)
{
    int port = 0;

    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else {
        if (client->mode != -1) {
            client->mode = -1;
            close(client->sock.fd);
        }
        client->sock.fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client->sock.fd == -1)
            return (0);
        port = rand() % 3000 + 1024;
        client->sock.my_addr = init_my_addr(port);
        init_socket_pasv(client);
        client->mode = 0;
        dprintf(client->fd, "227 Entering Passive Mode (127,0,0,1");
        dprintf(client->fd, ",%d,%d).\r\n", port / 256, port % 256);
    }
    return (0);
}

int str_digit(char *str)
{
    for (int i = 0; str[i]; i++)
        if (isdigit(str[i]) == 0)
            return (1);
    return (0);
}

int parse_port(char *ip)
{
    char *str = strdup(ip + 1);
    char **tab = NULL;
    int res = 0;

    str[strlen(str) - 1] = 0;
    tab = word_tab(str, ",");
    if (str_digit(tab[4]) == 1 || str_digit(tab[5]) == 1)
        return (-1);
    res = atoi(tab[4]) * 256 + atoi(tab[5]);
    my_free_tab(tab);
    free(str);
    return (res);
}

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

int port(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else if (my_strlen_tab(client->command) != 2)
        dprintf(client->fd, "501 Bad Argument.\r\n");
    else {
        if (client->mode != -1) {
            client->mode = -1;
            close(client->sock.fd);
        }
        port_check(client);
        dprintf(client->fd, "200 PORT okay.\r\n");
    }
    return (0);
}

int help(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else {
        dprintf(client->fd, "214 Command available: USER, PASS, CWD, CDUP");
        dprintf(client->fd, " QUIT, DELE, PWD, PASV, PORT, HELP, NOOP, RETR");
        dprintf(client->fd, " STOR, LIST.\r\n");
    }
    return (0);
}

int noop(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    dprintf(client->fd, "200 NOOP okay.\r\n");
    return (0);
}

void retr_exec(client_t *client, char *res)
{
    socklen_t lenght_socket;
    lenght_socket = sizeof(client->sock.my_addr);
    client->sock.fd_client = accept(client->sock.fd, (struct sockaddr *)
    &client->sock.my_addr, &lenght_socket);
    dprintf(client->sock.fd_client, "%s", res);
    dprintf(client->fd, "226 Closing data connection.\r\n");
    close(client->sock.fd);
    close(client->sock.fd_client);
}

int connection_abort(client_t *client)
{
    dprintf(client->fd, "451 Requested action aborted: local error in");
    dprintf(client->fd, " processing.\r\n");
    return (0);
}

int retr_connection(client_t *client)
{
    pid_t pid = 0;
    struct stat buffer;
    FILE *stream;
    char *res = NULL;

    if (stat(client->command[1], &buffer) != 0)
        return (connection_abort(client));
    res = malloc(sizeof(char) * (buffer.st_size + 1));
    stream = fopen(client->command[1], "r");
    fread(res, buffer.st_size + 1, 1, stream);
    res[buffer.st_size] = 0;
    dprintf(client->fd, "150 File status okay; about to open data");
    dprintf(client->fd, " connection.\r\n");
    pid = fork();
    if (pid == 0) {
        retr_exec(client, res);
        exit(0);
    }
    free(res);
    return (0);
}

void stor_exec(client_t *client, socklen_t lenght_socket)
{
    FILE *stream = NULL;
    char tmp[4096];
    char *res = NULL;
    int n = 0;

    lenght_socket = sizeof(client->sock.my_addr);
    client->sock.fd_client = accept(client->sock.fd, (struct sockaddr *)
    &client->sock.my_addr, &lenght_socket);
    stream = fopen(client->command[1], "w+");
    while ((n = read(client->sock.fd_client, tmp, 4096)) > 0) {
        res = dupcat(res, tmp, n);
        if (n < 4096) {
            printf("%d\n", n);
            break;
        }
    }
    fwrite(res, strlen(res) + 1, sizeof(char), stream);
    fclose(stream);
    dprintf(client->fd, "226 Closing data connection.\r\n");
}

int stor_connection(client_t *client)
{
    pid_t pid = 0;
    socklen_t lenght_socket = 0;

    dprintf(client->fd, "150 File status okay; about to open data");
    dprintf(client->fd, " connection.\r\n");
    pid = fork();
    if (pid == 0) {
        stor_exec(client, lenght_socket);
        close(client->sock.fd);
        close(client->sock.fd_client);
        client->mode = -1;
        exit(0);
    }
    return (0);
}

int retr(client_t *client)
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
        retr_connection(client);
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

int check_client(client_t *client)
{
    if (client->log != 1) {
        dprintf(client->fd, "530 Not Connected.\r\n");
        return (84);
    }
    if (client->mode == -1) {
        dprintf(client->fd, "425 Can't open data connection.\r\n");
        return (84);
    }
    return (0);
}

void list_data(client_t *client, char *res)
{
    socklen_t lenght_socket;

    lenght_socket = sizeof(client->sock.my_addr);
    client->sock.fd_client = accept(client->sock.fd, (struct sockaddr *)
    &client->sock.my_addr, &lenght_socket);
    dprintf(client->sock.fd_client, "%s", res);
    dprintf(client->fd, "226 Closing data connection.\r\n");
    close(client->sock.fd);
    close(client->sock.fd_client);
    client->mode = -1;
}

char *read_list(char *res, int link[2])
{
    int n = 0;
    char tmp[4096];

    while ((n = read(link[0], tmp, sizeof(tmp))) > 0) {
        res = dupcat(res, tmp, n);
        if (n < 4096)
            break;
    }
    return (res);
}

void exec_list(int link[2], client_t *client)
{
    dup2(link[1], STDOUT_FILENO);
    close(link[0]);
    close(link[1]);
    if (my_strlen_tab(client->command) == 1)
        execl("/bin/ls", "ls", "-l", (char *)0);
    if (my_strlen_tab(client->command) == 2)
        execl("/bin/ls", "ls", "-l", client->command[1], (char *)0);
}

int list(client_t *client)
{
    pid_t pid = 0;
    int link[2];
    char *res = NULL;

    if (check_client(client) == 84 || pipe(link) == -1 || (pid = fork()) == -1)
        return (0);
    if (pid == 0)
        exec_list(link, client);
    else
        res = read_list(res, link);
    wait(NULL);
    dprintf(client->fd, "150 File status okay; about to open data");
    dprintf(client->fd, " connection.\r\n");
    pid = fork();
    if (pid == 0) {
        list_data(client, res);
        exit(0);
    }
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
    printf("       port  is the port number on which the server socket");
    printf(" listens\n");
    printf("       path  is the path to the home directory for the Anonymous");
    printf(" user\n");
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

int choose_command(client_t *client)
{
    for (int i = 0; i < 14; i++) {
        if (strncmp(client->command[0], cmd[i], strlen(cmd[i])) == 0) {
            return (ptr[i](client));
        }
    }
    dprintf(client->fd, "500 Unknow command.\r\n");
    return (0);
}

int free_client(client_t *client)
{
    my_free_tab(client->command);
    free(client->path);
    free(client);
    return (0);
}

int exec_client_connection(int fd, char *path)
{
    FILE *stream = fdopen(fd, "r");
    char *str = NULL;
    size_t n;
    client_t *client = malloc(sizeof(client_t));

    client->fd = fd;
    client->log = -1;
    client->mode = -1;
    client->path = strdup(path);
    while (getline(&str, &n, stream) >= 0) {
        (str[strlen(str) - 1] == '\n') ? str[strlen(str) - 1] = '\0' : 0;
        (str[strlen(str) - 1] == '\r') ? str[strlen(str) - 1] = '\0' : 0;
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

int main(int ac, char **av)
{
    int my_socket = 0;
    fd_set current;
    fd_set ready;

    if (ac == 2 && strcmp(av[1], "-help") == 0)
        return (usage(av[0]));
    if (ac != 3 || chdir(av[2]) != 0)
        return (84);
    srand(time(NULL));
    if ((my_socket = init_server(atoi(av[1]))) == 0)
        return (84);
    FD_ZERO(&current);
    FD_SET(my_socket, &current);

    while (1) {
        ready = current;
        if (select(FD_SETSIZE, &ready, NULL, NULL, NULL) < 0)
            return (0);
        current = check_connection(current, ready, my_socket, av);
    }
    return (0);
}
