/*
 * Servidor Heat - Lógica Principal
 * Orquesta la comunicación por sockets y la simulación de calor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Incluimos las cabeceras de nuestros módulos
#include "heat.h"
#include "socket_utils.h"

// Buffer para los mensajes del cliente
char mess[100] = "";
// Buffer para la imagen a enviar al cliente
char image[SIZE];

/*
 * Hilo que gestiona la recepción de comandos del cliente.
 */
void *client_listener_thread(void* arg) {
    int n = 0;
    int coordX, coordY, tempe;
    // El argumento es el descriptor del socket del cliente
    long new_sock_id_long = (long)arg;
    int new_sock_id = (int)new_sock_id_long;

    do {
        bzero(mess, 100);
        n = read(new_sock_id, mess, 99); // Leemos hasta 99 para dejar espacio al \0
        if (n < 0) {
            perror("ERROR leyendo del socket");
            break;
        }
        printf("\nEl thread ha recibido %d bytes (%s)\n", n, mess);

        // Si el mensaje es un comando de calor, lo procesamos.
        if (n > 6 && mess[0] == '$') {
            sscanf(mess, "$%03d%03d%03d", &coordX, &coordY, &tempe);
            // Llamamos a la función de la biblioteca para aplicar el calor
            apply_heat_spot(coordX, coordY, tempe);
        }

    } while (strcmp(mess, "end") != 0);

    printf("\nThread terminando...\n");
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    printf("Abriendo puerto %d en espera de un cliente...\n", port);

    int client_sock_fd = open_server_socket(port);
    if (client_sock_fd < 0) {
        exit(EXIT_FAILURE);
    }
    printf("Cliente conectado.........\n");

    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, client_listener_thread, (void *)(long)client_sock_fd);
    printf("Thread de escucha iniciado.\n");

    initialize_heat_plate();
    
    printf("Iniciando bucle de simulación y envío de datos");
    fflush(stdout);

    do {
        compute_heat_step();
        copy_image_to_buffer(image);
        write(client_sock_fd, image, SIZE);

        // Añadimos un indicador visual para saber que el bucle está activo.
        printf(".");
        fflush(stdout);

        sleep(1); // Pequeña pausa para no saturar
    } while (strcmp(mess, "end") != 0);

    printf("\nBucle finalizado. Esperando el fin del thread...\n");
    pthread_join(listener_thread, NULL);
    close(client_sock_fd);
    printf("Servidor finalizado.\n");

    return EXIT_SUCCESS;
}