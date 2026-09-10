#include "socket_utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int open_server_socket(int port) {
    int on = 1;
    int sock_fd, new_sock_fd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("ERROR abriendo socket");
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR en bind");
        return -1;
    }

    listen(sock_fd, 5);
    clilen = sizeof(cli_addr);
    new_sock_fd = accept(sock_fd, (struct sockaddr *) &cli_addr, &clilen);
    if (new_sock_fd < 0) {
        perror("ERROR en accept");
        return -1;
    }
    
    // Devolvemos el descriptor del socket del cliente conectado.
    return new_sock_fd;
}