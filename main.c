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
        dprintf(client->fd, "331 %s okay, need password.\r\n", client->command[1]);
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
        if (ret == 0)
            dprintf(client->fd, "250 Requested file action okay, completed.\r\n");
        else
            dprintf(client->fd, "550 Failed to change directory.\r\n");
    }
    return (0);
}

int cdup(client_t *client)
{
    char path[256] = {0};
    int len = 0;
    char *new_dir = NULL;
    int ret = 0;

    if (client->log != 1) {
        dprintf(client->fd, "530 Not Connected.\r\n");
        return (0);
    }
    getcwd(path, sizeof(path));
    len = strlen(path);
    for (; len != 0 && path[len] != '/'; len--);
    new_dir = malloc(sizeof(char) * (len + 1));
    new_dir = strncpy(new_dir, path, len);
    new_dir[len] = '\0';
    if (strlen(new_dir) < strlen(client->path))
        ret = chdir(path);
    else
        ret = chdir(new_dir);
    if (ret == 0)
        dprintf(client->fd, "250 Directory successfully changed.\r\n");
    else
        dprintf(client->fd, "550 Requested action not taken.\r\n");
    free(new_dir);
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
    else if (my_strlen_tab(client->command) == 1 || my_strlen_tab(client->command) > 2)
        dprintf(client->fd, "501 Bad Argument.\r\n");
    else {
        if (remove(client->command[1]) == 0)
            dprintf(client->fd, "250 Requested file action okay, completed.\r\n");
        else
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

int pasv(client_t *client)
{
    uint16_t size = 0;
    int port = 0;
    int option = 1;
    pid_t pid = 0;
    socklen_t lenght_socket;

    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else {
        client->sock.fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client->sock.fd == -1) {
            perror("Socket");
            return (0);
        }
        client->sock.my_addr.sin_family = AF_INET;
        client->sock.my_addr.sin_addr.s_addr = INADDR_ANY;
        port = rand() % 3000 + 1024;
        client->sock.my_addr.sin_port = port;
        size = sizeof(client->sock.my_addr);
        if (setsockopt(client->sock.fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0) {
            perror("Setsockopt");
            return (0);
        }
        if (bind(client->sock.fd, (struct sockaddr *) &client->sock.my_addr, size) == -1) {
            perror("Bind");
            return (0);
        }
        if (listen(client->sock.fd, 100) == -1)
            perror("Listen");
        client->mode = 0;
        dprintf(client->fd, "227 Entering Passive Mode (127,0,0,1,%d,%d).\r\n", port / 256, port % 256);
        pid = fork();
        if (pid == 0) {
            lenght_socket = sizeof(client->sock.my_addr);
            client->sock.fd_client = accept(client->sock.fd, (struct sockaddr *) &client->sock.my_addr, &lenght_socket);
            exit(0);
        }
    }
    return (0);
}

/*int parse_ip(char *ip)
{
    char *str = strdup(ip);


}*/

/*int port_check(client_t *client)
{
    //getip()
    // COPY PASSV AVEC IP DONNEE
}*/

int port(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    else if (my_strlen_tab(client->command) != 1)
        dprintf(client->fd, "501 Bad Argument.\r\n");
    else
        dprintf(client->fd, "200 Command okay.\r\n");
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

int retr(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
    (void)client;
    return (0);
}

int stor(client_t *client)
{
    if (client->log != 1)
        dprintf(client->fd, "530 Not Connected.\r\n");
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
        dprintf(client->fd, "530 Not Connected.\r\n");
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
        if (my_strlen_tab(client->command) == 1)
            execl("/bin/ls", "ls", "-l", (char *)0);
        if (my_strlen_tab(client->command) == 2)
            execl("/bin/ls", "ls", "-l", client->command[1], (char *)0);
    } else {
        while ((n = read(link[0], tmp, sizeof(tmp))) > 0) {
            res = dupcat(res, tmp, n);
            if (n < 4096)
                break;
        }
        wait(NULL);
    }
    dprintf(client->fd, "150 File status okay; about to open data connection.\r\n");
    dprintf(client->fd, "%s", res);
    dprintf(client->fd, "226 Closing data connection.\r\n");
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

int init_server(int port)
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
    my_addr.sin_port = htons(port);
    lenght_socket = sizeof(my_addr);
    if (setsockopt(my_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0) {
        perror("Setsockopt");
        return (0);
    }
    if (bind(my_socket, (struct sockaddr *) &my_addr, lenght_socket) == -1) {
        perror("Bind");
        return (0);
    }
    if (listen(my_socket, 100) == -1)
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
    dprintf(client->fd, "500 Unknow command.\r\n");
    return (0);
}

int free_client(client_t *client)
{
    my_free_tab(client->env);
    my_free_tab(client->command);
    free(client->path);
    free(client);
    return (0);
}

int exec_client_connection(int fd, char **env, char *path)
{
    FILE *stream = fdopen(fd, "r");
    char *str = NULL;
    size_t n;
    client_t *client = malloc(sizeof(client_t));

    client->fd = fd;
    client->log = -1;
    client->mode = -1;
    client->env = my_arraycpy(env);
    client->path = strdup(path);
    while (getline(&str, &n, stream) >= 0) {
        if (str[strlen(str) - 1] == '\n')
            str[strlen(str) - 1] = '\0';
        if (str[strlen(str) - 1] == '\r')
            str[strlen(str) - 1] = '\0';
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
    else
        dprintf(socket_fd, "220 Service ready for new user.\r\n");
    return socket_fd;
}

struct sockaddr_in init_my_addr(int port)
{
    struct sockaddr_in my_addr;

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(port);
    return (my_addr);
}

int main(int ac, char **av, char **env)
{
    int my_socket = 0;
    int client_socket = 0;
    struct sockaddr_in my_addr;
    fd_set current;
    fd_set ready;
    pid_t pid = 0;

    if (ac == 2 && strcmp(av[1], "-help") == 0)
        return (usage(av[0]));
    if (ac != 3)
        return (84);
    srand(time(NULL));
    if (chdir(av[2]) != 0)
        return (84);
    my_addr = init_my_addr(atoi(av[1]));
    if ((my_socket = init_server(atoi(av[1]))) == 0)
        return (84);
    FD_ZERO(&current);
    FD_SET(my_socket, &current);

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
                    if (pid == 0) {
                        exec_client_connection(i, env, av[2]);
                        exit(0);
                    }
                    FD_CLR(i, &current);
                }
            }
        }
    }
    return (0);
}
