# OpenMP

En esta lección, los alumnos aprenden a programar con OpenMP, programando inicialmente un "Hola Mundo" en C en ALOE, para después trabajar con una implementación sencilla de la aplicación Wator. 

Los alumnos exploran los problemas del "false sharing" y otros tipos de fallos de coherencia.

```c
#include <omp.h>
#include <stdio.h>

      int main()
      {
              int x=0;
              //omp_set_num_threads(12);
#pragma omp parallel
            {
#pragma omp barrier
                      printf("hola");
#pragma omp barrier
                int id=omp_get_thread_num();
#pragma omp atomic
                x=x+id;
                printf("id x  = %d %d\n",id,x);
#pragma omp barrier
              }

                printf("x  =  %d\n",x);
      }
```
