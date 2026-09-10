#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main() {
    // Declaramos la variable ID fuera de las regiones paralelas para que sea visible globalmente.
    int ID;

    // Primera región paralela con la directiva omp parallel
    #pragma omp parallel private(ID) // "private" asegura que cada hilo tenga su propia copia de ID
    {
        ID = omp_get_thread_num(); // Asignación del número de hilo a ID
        printf("hello(%d)", ID);
        printf("world(%d)\n", ID);
    }

    // Segunda región paralela con un bucle for
    #pragma omp parallel for shared(ID) // "shared" permite que todos los hilos accedan a la misma ID
    for (int i = 0; i < 100; i++) {
        int localID = omp_get_thread_num(); // Usamos una variable local para obtener el ID del hilo
        printf("%02d %02d\n", localID, i);
    }

    // Ejemplo de directiva omp single
    #pragma omp parallel
    {
        #pragma omp single // Solo un hilo ejecutará esta sección
        {
            printf("Solo un hilo ejecuta esto, ID: %d\n", omp_get_thread_num());
        }
    }

    // Ejemplo de directiva omp critical para evitar condiciones de carrera
    #pragma omp parallel private(ID)
    {
        ID = omp_get_thread_num();
        #pragma omp critical // Solo un hilo puede acceder a esta sección a la vez
        {
            printf("En región crítica, ID: %d\n", ID);
        }
    }

    // Ejemplo de omp barrier para sincronizar hilos
    #pragma omp parallel
    {
        #pragma omp barrier // Espera a que todos los hilos lleguen aquí
        printf("Todos los hilos sincronizados aquí, ID: %d\n", omp_get_thread_num());
    }

    return 0;
}
 