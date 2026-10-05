/**
 * @file slave.c
 * @brief Aplicación servidor (esclavo) para comunicación mediante sockets TCP y pthreads.
 * 
 * Abre un socket en modo servidor, espera una conexión entrante y delega la
 * lectura de datos en un hilo secundario mientras el hilo principal simula
 * trabajo en segundo plano, sincronizándose mediante un mutex.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 128
#define BACKLOG 5

/* Búfer compartido y cerrojo de exclusión mutua para evitar condiciones de carrera */
static char mess[BUFFER_SIZE] = "";
static pthread_mutex_t mess_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Crea el socket de escucha, enlaza el puerto y acepta un cliente.
 * 
 * @param puerto Puerto TCP donde escuchar.
 * @return int Descriptor del socket del cliente conectado, o -1 en caso de error.
 */
int open_socket(int puerto) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error al crear el socket del servidor");
        return -1;
    }

    // Permite reutilizar el puerto inmediatamente tras cerrar el proceso
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Error en setsockopt (SO_REUSEADDR)");
        close(server_fd);
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(puerto);

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error en bind (el puerto puede estar en uso)");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("Error en listen");
        close(server_fd);
        return -1;
    }

    printf("Servidor a la escucha en el puerto %d...\n", puerto);

    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &clilen);
    if (client_fd < 0) {
        perror("Error en accept");
        close(server_fd);
        return -1;
    }

    char ip_cliente[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(cli_addr.sin_addr), ip_cliente, INET_ADDRSTRLEN);
    printf("Cliente conectado desde %s:%d\n", ip_cliente, ntohs(cli_addr.sin_port));

    // Tras aceptar al cliente, cerramos el socket de escucha para no fugar descriptores
    close(server_fd);

    return client_fd;
}

/**
 * @brief Hilo encargado de la recepción de datos por el socket.
 * 
 * Lee continuamente los mensajes enviados por el cliente y los almacena
 * de forma segura en la variable compartida 'mess'.
 * 
 * @param arg Descriptor del socket cliente casteado a void*.
 */
void *funcion_hilo(void *arg) {
    int client_fd = (int)(intptr_t)arg;
    char buffer_local[BUFFER_SIZE];

    while (1) {
        memset(buffer_local, 0, sizeof(buffer_local));
        ssize_t n = read(client_fd, buffer_local, sizeof(buffer_local) - 1);

        if (n <= 0) {
            if (n < 0) {
                perror("\n[Hilo] Error leyendo del socket");
            } else {
                printf("\n[Hilo] Conexión cerrada por el cliente.\n");
            }

            pthread_mutex_lock(&mess_mutex);
            strncpy(mess, "end", sizeof(mess) - 1);
            pthread_mutex_unlock(&mess_mutex);
            break;
        }

        printf("\n[Hilo] Ha recibido %zd bytes: \"%s\"\n", n, buffer_local);

        pthread_mutex_lock(&mess_mutex);
        strncpy(mess, buffer_local, sizeof(mess) - 1);
        mess[sizeof(mess) - 1] = '\0';
        int terminar = (strcmp(mess, "end") == 0);
        pthread_mutex_unlock(&mess_mutex);

        if (terminar) {
            break;
        }
    }

    printf("[Hilo] Finalizando ejecución...\n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Modo de empleo: %s <puerto>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int puerto = atoi(argv[1]);
    int new_sock_id = open_socket(puerto);
    if (new_sock_id < 0) {
        return EXIT_FAILURE;
    }

    pthread_t mithread;
    if (pthread_create(&mithread, NULL, funcion_hilo, (void *)(intptr_t)new_sock_id) != 0) {
        perror("Error al crear el hilo");
        close(new_sock_id);
        return EXIT_FAILURE;
    }
    printf("Hilo receptor iniciado con éxito.\n");

    /* El hilo principal procesa datos concurrentemente mientras el hilo secundario atiende el socket */
    int terminado = 0;
    do {
        char mensaje_actual[BUFFER_SIZE];

        pthread_mutex_lock(&mess_mutex);
        strncpy(mensaje_actual, mess, sizeof(mensaje_actual) - 1);
        mensaje_actual[sizeof(mensaje_actual) - 1] = '\0';

        // Si hay un mensaje pendiente que no sea "end", lo consumimos vaciándolo
        if (strcmp(mess, "end") != 0) {
            mess[0] = '\0';
        }
        pthread_mutex_unlock(&mess_mutex);

        if (strcmp(mensaje_actual, "end") == 0) {
            terminado = 1;
        } else if (mensaje_actual[0] == '\0') {
            printf(".");
            fflush(stdout);
        } else {
            printf(" %s\n", mensaje_actual);
            fflush(stdout);
        }

        if (!terminado) {
            sleep(1);
        }
    } while (!terminado);

    printf("\nEsperando el fin del hilo...\n");
    pthread_join(mithread, NULL);

    printf("Cerrando conexión del cliente...\n");
    close(new_sock_id);

    return EXIT_SUCCESS;
}
