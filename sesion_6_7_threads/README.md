# Los hilos o threads

En sistemas operativos, un hilo de ejecución, hebra o subproceso es la unidad de procesamiento
más pequeña que puede ser planificada por un sistema operativo. La creación de un nuevo hilo es
una característica que permite a una aplicación realizar varias tareas a la vez (concurrentemente).
Los distintos hilos de ejecución comparten una serie de recursos tales como el espacio de
memoria, los archivos abiertos, situación de autenticación, etc. Esta técnica permite simplificar el
diseño de una aplicación que debe llevar a cabo distintas funciones simultáneamente. Un hilo es
simplemente una tarea que puede ser ejecutada al mismo tiempo con otra tarea. 

Los hilos de ejecución que comparten los mismos recursos, sumados a estos recursos, son en
conjunto conocidos como un proceso. Una definición más simple de proceso es "un programa en
ejecución”. El hecho de que los hilos de ejecución de un mismo proceso compartan los recursos
hace que cualquiera de estos hilos pueda modificar éstos. Cuando un hilo modifica un dato en la
memoria, los otros hilos acceden a ese dato modificado inmediatamente. 

Lo que es propio de cada hilo es el contador de programa, la pila de ejecución y el estado de la
CPU (incluyendo el valor de los registros). 

El proceso sigue en ejecución mientras al menos uno de sus hilos de ejecución siga activo. Cuando
el proceso finaliza, todos sus hilos de ejecución también han terminado (o se terminan). Asimismo en el momento
en el que todos los hilos de ejecución finalizan, el proceso no existe más y todos sus recursos son
liberados. 

## Los hilos POSIX (pthreads)

El Sistema Operativo Unix/Linux ya dispone de funciones (fork o clone) que permiten que un
proceso se replique. A partir del punto en el que se llama a la función, el sistema operativo los
considera como dos tareas casi independientes (por ejemplo, en el reparto de tiempo de CPU
mediante time slicing). Sin embargo, la creación de hilos, la comunicación entre ellos y la propia compartición de memoria es más compleja
cuando se implementa mediante "llamadas al sistema" (traps, a bajo nivel), por lo que lo más habitual es utilizar
librerías de más alto nivel. Entre ellas, las más conocidas son las librerías POSIX-threads (o
pthreads) y OpenMP.

## Creación de un Pthread 

El siguiente código muestra la creación de un hilo con la librería pthread. Averigüe cuál es el
proceso principal y la tarea del hilo creado. Averigüe para qué sirve la función join. A continuación,
compílelo (añada la opción -lpthread) y ejecútelo.

```C
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
//Only a thread

void *thread_func(void* argv)
{
sleep(5);
printf("Hello world\n");
}

main(int argc, char *argv[])
{
int i;
pthread_t mythread,my2;    /* thread and attributes */
pthread_create(&mythread,NULL, thread_func,NULL);
for(i=9;i>=0;i--){
        printf("%d\n",i);
        sleep(1);
        }
pthread_join(mythread,NULL);
}

```

Para compilar, utilice el comando:

```shell
gcc -o mainX.exe mainX.c -pthread
```

## Breve descripción de los códigos agregados

1. [main1.c](main1.c): Creación de dos hilos.
2. [main2.c](main2.c): Creación de dos hilos y uso de memoria para intercambio.
3. [main3.c](main3.c): Dependencia verdadera y sincronización con semáforos.
4. [main4.c](main4.c): Sincronización sin bloqueos (try wait).
5. [main5.c](main5.c): El problemas RAW/WAR en un lazo.
6. [main6.c](main6.c): El problemas RAW/WAR en un lazo.
7. [main7.c](main7.c): Sincronización con mutex.
8. [main8.c](main8.c): Barreras con mutex.
9. [main9.c](main9.c): Comparación de rendimientos entre semáforos y mutex.