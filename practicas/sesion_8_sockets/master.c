/**
 * @file master.c
 * @brief Aplicación cliente (maestro) para comunicación mediante sockets TCP.
 * 
 * Conecta con un esclavo (servidor), envía cadenas leídas desde la entrada
 * estándar y finaliza la conexión al enviar la palabra clave "end".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 128

/**
 * @brief Resuelve la dirección del host y se conecta al servidor TCP.
 * 
 * @param host Dirección IP (ej. "127.0.0.1") o nombre de dominio (ej. "localhost").
 * @param puerto Puerto TCP en el que escucha el servidor.
 * @return int Descriptor del socket conectado, o -1 en caso de fallo.
 */
int open_socket_client(const char *host, int puerto) {
    char puerto_str[16];
    snprintf(puerto_str, sizeof(puerto_str), "%d", puerto);

    struct addrinfo hints, *res, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP

    int status = getaddrinfo(host, puerto_str, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "Error resolviendo el host '%s': %s\n", host, gai_strerror(status));
        return -1;
    }

    int sock_fd = -1;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock_fd == -1) {
            continue;
        }

        if (connect(sock_fd, rp->ai_addr, rp->ai_addrlen) != -1) {
            break; // Conexión exitosa
        }

        close(sock_fd);
        sock_fd = -1;
    }

    freeaddrinfo(res);

    if (sock_fd == -1) {
        perror("Error al conectar con el servidor (socket no disponible)");
        return -1;
    }

    return sock_fd;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Modo de empleo: %s <IP_o_host> <puerto>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *host = argv[1];
    int puerto = atoi(argv[2]);

    printf("Abriendo conexión con %s en el puerto %d...\n", host, puerto);
    int sock_fd = open_socket_client(host, puerto);
    if (sock_fd < 0) {
        return EXIT_FAILURE;
    }

    printf("Conectado con éxito.\n");
    printf("Escriba palabras y pulse INTRO. Para terminar, escriba \"end\":\n");

    char mess[BUFFER_SIZE];
    while (1) {
        printf("> ");
        fflush(stdout);

        // Lectura protegida contra desbordamiento de búfer
        if (scanf("%127s", mess) != 1) {
            break; // EOF o error en entrada
        }

        // Enviamos la palabra incluyendo el terminador '\0'
        size_t len = strlen(mess) + 1;
        if (write(sock_fd, mess, len) < 0) {
            perror("Error escribiendo en el socket");
            break;
        }

        if (strcmp(mess, "end") == 0) {
            break;
        }
    }

    printf("Cerrando socket...\n");
    close(sock_fd);

    return EXIT_SUCCESS;
}
