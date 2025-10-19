#include "heat.h"
#include <stdio.h>

// Definición de la variable global que contendrá la matriz de temperatura.
double w[SIZEY][SIZEX];

// Renombramos 'iniheat' a un nombre más descriptivo.
void initialize_heat_plate() {
    int i, j;
    double mean = 0.0;

    // Establece las condiciones de contorno (bordes de la placa)
    for (i = 1; i < SIZEY - 1; i++) {
        w[i][0] = 100.0;
        w[i][SIZEX - 1] = 100.0;
    }
    for (j = 0; j < SIZEX; j++) {
        w[SIZEY - 1][j] = 100.0;
        w[0][j] = 0.0;
    }

    // Calcula la media de las temperaturas de los bordes
    for (i = 1; i < SIZEY - 1; i++) {
        mean += w[i][0];
        mean += w[i][SIZEX - 1];
    }
    for (j = 0; j < SIZEX; j++) {
        mean += w[SIZEY - 1][j];
        mean += w[0][j];
    }
    mean = mean / (double)(2 * SIZEY + 2 * SIZEX - 4);

    // Inicializa el interior de la placa con el valor medio.
    for (i = 1; i < SIZEY - 1; i++) {
        for (j = 1; j < SIZEX - 1; j++) {
            w[i][j] = mean;
        }
    }
}

// Renombramos 'calienta' para mayor claridad.
void apply_heat_spot(int x, int y, int temp) {
    int size = 5;
    int i, j;

    // Aplica temperatura en un área cuadrada alrededor del punto (x, y)
    for (i = y - size; i <= y + size; i++) {
        for (j = x - size; j <= x + size; j++) {
            // Comprueba que no nos salimos de los límites de la matriz
            if ((i > -1) && (i < SIZEY) && (j > -1) && (j < SIZEX)) {
                w[i][j] = (double)temp;
            }
        }
    }
}

// Renombramos 'heat' a un nombre más explícito.
void compute_heat_step() {
    // u: copia de la matriz del paso anterior para el cálculo.
    double u[SIZEY][SIZEX];
    int i, j;

    // 1. Copiar el estado actual (w) a la matriz temporal (u)
    for (i = 0; i < SIZEY; i++) {
        for (j = 0; j < SIZEX; j++) {
            u[i][j] = w[i][j];
        }
    }

    // 2. Calcular el nuevo estado de 'w' basándose en los vecinos de 'u'.
    //    Esto evita que los cálculos de un punto afecten a sus vecinos en la misma iteración.
    for (i = 1; i < SIZEY - 1; i++) {
        for (j = 1; j < SIZEX - 1; j++) {
            w[i][j] = (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]) / 4.0;
        }
    }
}

// Renombramos 'copyimage' y hacemos que reciba el buffer de destino.
void copy_image_to_buffer(char* image_buffer) {
    int i, j;
    for (i = 0; i < SIZEY; i++) {
        for (j = 0; j < SIZEX; j++) {
            // Convierte el valor double a char (0-255) y lo guarda en el buffer.
            image_buffer[i * SIZEX + j] = (char)w[i][j];
        }
    }
}

// Función de utilidad para medir el tiempo, si fuera necesaria fuera de la biblioteca.
double get_cpu_time(void) {
    return (double)clock() / (double)CLOCKS_PER_SEC;
}