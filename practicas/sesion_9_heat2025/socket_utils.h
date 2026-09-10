#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

// Abre un socket en modo servidor y espera una conexión de un cliente.
int open_server_socket(int port);

#endif // SOCKET_UTILS_H