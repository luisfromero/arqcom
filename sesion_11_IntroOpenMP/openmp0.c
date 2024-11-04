#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main() {
    
    return 0;
    #pragma omp parallel private(ID) // "private" asegura que cada hilo tenga su propia copia de ID
    {
    printf("hello world(%d)\n", ID);
    }

    
    
    // Declaramos la variable ID fuera de las regiones paralelas para que sea visible globalmente.
    int ID;



    // Primera región paralela con la directiva omp parallel
    #pragma omp parallel private(ID) // "private" asegura que cada hilo tenga su propia copia de ID
    {
        ID = omp_get_thread_num(); // Asignación del número de hilo a ID
        if(ID%2==0)printf("hello(%d)", ID);
        if(ID%2==1)printf("world(%d)\n", ID);
    }


    return 0;
}
