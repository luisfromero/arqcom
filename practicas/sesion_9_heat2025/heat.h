#ifndef HEAT_H
#define HEAT_H

#include <time.h>

#define SIZEX 500
#define SIZEY 500
#define SIZE (SIZEX * SIZEY)

/*
 * Declaramos la matriz de temperatura como 'extern'.
 * Esto significa que la variable existe en algún otro archivo (.c)
 * y que queremos acceder a ella.
 */
extern double w[SIZEY][SIZEX];

// --- Interfaz Pública de la Biblioteca ---

// Inicializa la matriz de calor con las condiciones de contorno.
void initialize_heat_plate();

// Aplica un punto de calor en una coordenada específica.
void apply_heat_spot(int x, int y, int temp);

// Ejecuta un paso de la simulación de la ecuación de calor.
void compute_heat_step();

// Copia la matriz de temperatura (double) a un buffer de visualización (char).
void copy_image_to_buffer(char* image_buffer);

#endif // HEAT_H