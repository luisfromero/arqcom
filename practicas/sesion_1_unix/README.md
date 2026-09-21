                      # Práctica 1: conexión remota y primeros pasos en Unix

                      Apuntes ordenados para arrancar rápido en la asignatura: conexión al servidor, comandos básicos, sistema de archivos y compilación en C.

                      ## 1) Entorno de trabajo

                      Servidor de prácticas (ejemplo):

                      - Nombre: [aloe.ac.uma.es](http://aloe.ac.uma.es/)
                      - IP: 150.214.109.24
                      - SO: Ubuntu 18.04.4 LTS

                      Cliente recomendado para Windows:

                      - MobaXterm: https://mobaxterm.mobatek.net/download-home-edition.html

                      Que aporta MobaXterm:

                      - Terminal SSH
                      - Cliente SFTP integrado para mover archivos
                      - Servidor X11 para abrir aplicaciones gráficas remotas

                      ## 2) TCP/IP explicado para esta práctica

                      Cuando te conectas por SSH, por debajo ocurre esto:

                      1. Tu equipo abre una conexión TCP hacia el servidor.
                      2. TCP asegura entrega ordenada y fiable de los datos.
                      3. Sobre TCP viaja SSH, que cifra toda la sesión.
                      4. Si hay salto intermedio (por ejemplo, login), se crea un túnel hasta la máquina destino.

                      Conceptos clave:

                      - IP identifica máquinas (direcciones como 150.214.109.24).
                      - Puerto identifica servicios dentro de una máquina.
                      - SSH suele usar el puerto 22/TCP.
                      - TCP usa confirmaciones y retransmisión para evitar pérdida de datos.

                      Comprobaciones útiles:

                      ```bash
                      ping aloe.ac.uma.es
                      traceroute aloe.ac.uma.es
                      ssh usuario@aloe.ac.uma.es
                      ```

                      ## 3) Primer acceso remoto

                      Ejemplo de conexión:

                      ```bash
                      ssh usuario@aloe.ac.uma.es
                      ```

                      Si hay máquina intermedia:

                      ```bash
                      ssh usuario@login.ac.uma.es
                      ssh usuario@aloe.ac.uma.es
                      ```

                      Consejo: las credenciales de alumno no deben guardarse en apuntes públicos. Mejor mantenerlas en el campus virtual o en un gestor seguro.

                      ## 4) Comandos Unix esenciales

                      Identidad y sesión:

                      ```bash
                      who
                      whoami
                      tty
                      ```

                      Rutas y ayuda:

                      ```bash
                      pwd
                      man ls
                      echo "hola"
                      ```

                      Archivos:

                      ```bash
                      ls -la
                      cat fichero.txt
                      more fichero.txt
                      ```

                      Hardware (si tienes permisos):

                      ```bash
                      lshw
                      ```

                      ## 5) Sistema de archivos y rutas

                      Símbolos básicos:

                      - . directorio actual
                      - .. directorio padre
                      - / raíz del sistema

                      Comandos típicos:

                      ```bash
                      mkdir practica1
                      cd practica1
                      touch notas.txt
                      cp notas.txt copia.txt
                      mv copia.txt copia_v2.txt
                      rm copia_v2.txt
                      ```

                      ## 6) Compilación básica con gcc

                      Compilador: [GNU Compiler Collection](https://gcc.gnu.org/)

                      Fases de compilación:

                      1. Preprocesado y compilación a ensamblador.
                      2. Ensamblado a objeto (.o).
                      3. Enlazado para generar ejecutable.

                      Comandos:

                      ```bash
                      gcc -S main.c          # genera main.s
                      gcc -c main.c          # genera main.o
                      gcc main.c             # compila y enlaza (a.out por defecto)
                      gcc -o main main.c     # nombre explícito de salida
                      ```

                      Múltiples archivos fuente:

                      ```bash
                      gcc main.c rutina1.c rutina2.c -o programa
                      ```

                      Inspección de binarios:

                      ```bash
                      hexdump main.o
                      objdump -d main.o
                      ```

                      ## 7) Compilación nativa vs cruzada

                      - Nativa: compilas para la misma arquitectura donde ejecutas.
                      - Cruzada: compilas para otra arquitectura destino.

                      Ejemplo nativo:

                      ```bash
                      gcc main0.c -o main0
                      ./main0
                      ```

                      Ejemplo cruzado a MIPS:

                      ```bash
                      mips-linux-gnu-gcc -c main0.c
                      mips-linux-gnu-objdump -d main0.o
                      ```

                      ## 8) Mini práctica de optimización

                      Pruebas sugeridas:

                      ```bash
                      mips-linux-gnu-gcc -c -O0 -save-temps datos.c loop.c main.c
                      mips-linux-gnu-gcc -c -O5 -save-temps datos.c loop.c main.c
                      mips-linux-gnu-gcc -c -Ofast -funroll-all-loops -save-temps datos.c loop.c main.c
                      ```

                      Fíjate en:

                      - Cambios en los .s generados
                      - Diferencias de tamaño de objetos
                      - Transformaciones del bucle (loop unrolling)

                      ## 9) Introducción breve a hilos (pthreads)

                      Un hilo es una unidad ligera de ejecución dentro de un proceso. Varios hilos comparten memoria y recursos del proceso.

                      Compilación con pthread:

                      ```bash
                      gcc -o mainX mainX.c -pthread
                      ```

                      Objetivo del ejercicio:

                      - Identificar hilo principal y secundario
                      - Entender por qué usar pthread_join para sincronizar

                      ## Checklist rápido

                      - Me conecto por SSH sin errores.
                      - Entiendo qué papel tiene TCP en la conexión.
                      - Manejo comandos básicos de Unix.
                      - Sé crear y mover archivos/carpetas.
                      - Compilo en C en modo nativo.
                      - Distingo compilación nativa y cruzada.

  

Descripción de los códigos agregados
------------------------------------

1.  **main1.c**: Creación de dos hilos.
2.  **main2.c**: Creación de dos hilos y uso de memoria para intercambio.
3.  **main3.c**: Dependencia verdadera y sincronización con semáforos.
4.  **main4.c**: Sincronización sin bloqueos (try wait).
5.  **main5.c**: El problemas RAW/WAR en un lazo.
6.  **main6.c**: El problemas RAW/WAR en un lazo.
7.  **main7.c**: Sincronización con mutex.
8.  **main8.c**: Barreras con mutex.
9.  **main9.c**: Comparación de rendimientos entre semáforos y mutex.

Práctica de comunicación entre aplicaciones por sockets
-------------------------------------------------------

Descargue el resto de archivos para esta práctica, [de éste enlace](http://casium.uma.es/descargas/codigosSockets.zip).

Socket designa un concepto abstracto por el cual dos programas (posiblemente situados en computadoras distintas) pueden intercambiar cualquier flujo de datos. El término socket es también usado como el nombre de una interfaz de programación de aplicaciones (API) para la familia de protocolos de Internet TCP/IP, provista usualmente por el sistema operativo.

Los sockets de Internet constituyen el mecanismo para la entrega de paquetes de datos provenientes de la tarjeta de red a los procesos o hilos apropiados. Un socket queda definido por un par de direcciones IP local y remota, un protocolo de transporte y un par de números de puerto local y remoto.

### El maestro

En esta práctica, existe un programa (master.c) que se comporta como aplicación _maestro_ (la que “manda” en la relación), aunque también se puede considerar como _cliente_ (el que solicita un servicio).

pre{ color:darkgreen; } span.green{ color:green; } span.red{ color:red; } span.blue{ color:blue; }

Estos son los includes necesarios, así como el espacio en el que prepara "el encargo" (mess):

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

char mess\[100\]="";

El cliente entra en un bucle "casi infinito" en el que, en cada iteración, envía un "trabajo", en forma de texto (mediante la escritura (_write_) en el socket:

main(int argc, char \*argv\[\]) {
int i;
int new\_sock\_id;
if(argc<3){printf("Modo de uso:  master IP port\\n");exit(0);}
        printf("Opening port %d in %s..\\n",atoi(argv\[2\]),argv\[1\]);
        new\_sock\_id=open\_socket\_client(atoi(argv\[2\]),argv\[1\]);
        printf("Conectado. Write words (and enter). To finish, write \\"end\\"\\n");
        do{
            scanf("%s",mess);
            write(new\_sock\_id, mess, 1+strlen(mess));
        }while(strcmp(mess,"end"));
printf("Closing socket...\\n");
close(new\_sock\_id);
}

Observe los siguientes detalles.

*   La línea en azul indica que al ejecutar la aplicación debes indicar la dirección IP de la máquina a la que te tienes que conectar por sockets, así como el número de puerto. En internet, las direcciones IP identifican a cada máquina del nodo, mientras que el número de puerto es una forma de identificar a las aplicaciones dentro de la máquina.
*   En la segunda línea en rojo, El cliente establece la conexión a través de un socket. En la máquina (y puerto) destino, debe haber previamente un esclavo “a la escucha”, en ese puerto, para que la llamada tenga éxito. En caso contrario, se queda bloqueado.
*   En la segunda línea en verde, el cliente envía paquetes de texto (mess) que previamente ha leído del teclado (scanf).
    
    finalmente, este es el código con el que se "enchufa" el socket:
    
    int open\_socket\_client(int puerto, char \*host)
    {
    int fd;
    struct hostent \*he;
    struct sockaddr\_in server;
    
    if((he=gethostbyname(host))==NULL){
    	printf("Error gethosbyname\\n" ); exit(-1); }
    if( ( fd=socket(AF\_INET, SOCK\_STREAM,0) ) ==-1) {
    	printf("Error creando  socket\\n"); exit(-1); }
    
    server.sin\_family=AF\_INET;
    server.sin\_port=htons(puerto);
    server.sin\_addr=\*((struct in\_addr\*) he->h\_addr);
    bzero(&(server.sin\_zero),0);
    
    if(connect(fd,(struct sockaddr\*)&server, sizeof(struct sockaddr))==-1){
    	printf("Error al conectar. Socket no disponible\\n"); exit(-1); } 
    return fd;
    }
    

### El esclavo o servidor

El servidor es el que realiza el trabajo solicitado por un cliente. Esta aplicación es la primera que se pone en marcha, y en nuestro caso, se ha diseñado para ejecutar un único trabajo.

Además, el servidor necesita mantener viva la comunicación con el cliente, a la vez que hace el trabajo, por lo que crea un thread para ello:

Estos son los includes necesarios, así como el espacio en el que recibe "el encargo":

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

char mess\[100\]="";

Este código es el que utiliza para esperar una conexión (preparamos el socket):

int  open\_socket(int puerto, int id){
	int on = 1;
	int newsockfd, portno, clilen;
	char buffer\[256\],test;
	struct sockaddr\_in serv\_addr, cli\_addr;
	int n;
	id = socket(AF\_INET, SOCK\_STREAM, 0);
	if (id < 0) printf("ERROR opening socket\\n");
	bzero((char \*) &serv\_addr, sizeof(serv\_addr));
	portno = (int) puerto;
	serv\_addr.sin\_family = AF\_INET;
	serv\_addr.sin\_addr.s\_addr = INADDR\_ANY;
	serv\_addr.sin\_port = htons(portno);
	setsockopt(id, SOL\_SOCKET, SO\_REUSEADDR, &on, sizeof( on)) ;
	if (bind(id, (struct sockaddr \*) &serv\_addr, sizeof(serv\_addr)) < 0) printf("ERROR binding\\n");

	listen(id,5);
	clilen = sizeof(cli\_addr);
	newsockfd = accept(id, (struct sockaddr \*) &cli\_addr, &clilen);
	if (newsockfd < 0) printf("ERROR aceptando\\n");
	return newsockfd;
	}

Este código del thread que se encarga de esperar los "encargos", y sólo hasta que reciba un último encargo, con el texto _end_:

void \*funcion\_hilo(void\* new\_sock\_id) {
int n=0;
	do{
		sleep(1);
     		bzero(mess,100);
     		n = read(new\_sock\_id,mess,100);
		printf("\\nEl thread ha recibido  %d bytes:",n);
     		if (n < 0) perror("ERROR leyendo del socket");
	}while(strcmp(mess,"end"));
printf("\\nThread terminando...\\n");
pthread\_exit(NULL);
return 0;
	}

Finalmente, el hilo principal, una vez establecida la comunicación, realiza su tarea dentro de otro bucle infinito _do-while_ en la que hace su trabajo actual, representado por printf("."), así como posteriores encargos, a través de la variable _mess_ que le llega por el socket:

main(int argc, char \*argv\[\]) {
int i;
pthread\_t mithread;    /\* thread and attributes \*/
int sock\_id, new\_sock\_id;
if(argc<2){printf("Modo de empleo: slave port\\n");exit(0);}
	printf("Abriendo puerto...\\n");
	new\_sock\_id=open\_socket(atoi(argv\[1\]),sock\_id);
	printf("Slave conectado.........\\n");
	pthread\_create(&mithread,NULL, funcion\_hilo,(void \*) (new\_sock\_id));
	printf("Thread iniciado \\n");
	do{
		if(mess\[0\]==0)printf(".");
		else printf(" %s\\n",mess);
		fflush(0);
		sleep(1);
	}while(strcmp(mess,"end"));
printf("Esperando el fin del thread...\\n");
pthread\_join(mithread,NULL);
close(sock\_id);
}

Servidor HEAT

En esta práctica pondremos en marcha un servidor que resuelve la ecuación de difusión, Y que simula la temperatura de una plancha cuadrada. Los encargos del cliente consisten en cambios puntuales de la temperatura en un sitio concreto de la plancha.

Este es el código del servidor:

/\* 
 \* File:   main.c
 \* Author: felipe
 \*
 \* Created on 1 de diciembre de 2015, 13:27
 \* Modificado el 12 de noviembre de 2020
 \* 
 \* Servidor Heat. Se inicia en modo de espera de un cliente
 \* 
 \* Cuando se establece la conexion, crea un thread que espera la llegada de comandos.
 \* El hilo principal envia paquetes de SIZE bytes, con los resultados del servicio 
 \* El servicio ejecuta un paso del método explicito de resolucion por diferencias finitas
 \* de la eq de calor con un coeficiente de difusion arbitrario.
 \*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

//OpenMP Será necesario más adelante
#include <omp.h>

#define SIZEX 500
#define SIZEY 500
#define SIZE SIZEX\*SIZEY 
//Variable global, compartida, con la temperatura 
double w\[SIZEY\]\[SIZEX\];  //Espacio para guardar los 250000 valores de temperatura (double)
char mess\[100\]="";
//Espacio para guardar la informacion de temperatura compacta (1 byte por punto, 250000 elementos)
char image\[SIZE\];




//Mide de forma precisa el tiempo de ejeq
double cpu\_time ( void )
{
  return ( double ) clock ( ) / ( double ) CLOCKS\_PER\_SEC;
}




//Establece una condicion de contorno Dirichlet
//          Frio
//Calor              Calor
//          Calor
int iniheat()
{
    int i,j;
    double mean;
      for ( i = 1; i < SIZEY - 1; i++ )
  {
    w\[i\]\[0\] = 100.0;
  }
  for ( i = 1; i < SIZEY - 1; i++ )
  {
    w\[i\]\[SIZEX-1\] = 100.0;
  }
  for ( j = 0; j < SIZEX; j++ )
  {
    w\[SIZEY-1\]\[j\] = 100.0;
  }
  for ( j = 0; j < SIZEX; j++ )
  {
    w\[0\]\[j\] = 0.0;
  }
/\*
   Calcula un valor interpolado en la zona interior
\*/
  mean = 0.0;
  for ( i = 1; i < SIZEY - 1; i++ )
  {
    mean = mean + w\[i\]\[0\];
  }
  for ( i = 1; i < SIZEY - 1; i++ )
  {
    mean = mean + w\[i\]\[SIZEX-1\];
  }
  for ( j = 0; j < SIZEX; j++ )
  {
    mean = mean + w\[SIZEY-1\]\[j\];
  }
  for ( j = 0; j < SIZEX; j++ )
  {
    mean = mean + w\[0\]\[j\];
  }
  mean = mean / ( double ) ( 2 \* SIZEY + 2 \* SIZEX - 4 );
/\* 
  Initialize the interior solution to the mean value.
\*/
  for ( i = 1; i < SIZEY - 1; i++ )
  {
    for ( j = 1; j < SIZEX - 1; j++ )
    {
      w\[i\]\[j\] = mean;
    }
  }
 
}




int coordX,coordY,tempe;  //Datos que se reciben del cliente por un thread separado

void \*funcion\_hilo(void\* new\_sock\_id) {
int n=0;

do{
//sleep(1);
     bzero(mess,100);
     n = read(new\_sock\_id,mess,100);
printf("\\nEl thread ha recibido  %d bytes (%s)\\n",n,mess);
     if (n < 0) perror("ERROR leyendo del socket");
                        if(n>6)if(mess\[0\]=='$'){
                            sscanf(mess,"$%03d%03d%03d",&coordX,&coordY,&tempe);
                            w\[coordY\]\[coordX\]=tempe;
                            //El cliente cambia la temperatura de un determinado punto
                            //El alumno puede mejorar esta operacion, creando una funcion void calienta(int x, int y, int z);
                            //calienta(coordX,coordY,tempe); No solo actualiza w en un punto. En los vecinos tambien
                        }
                        
}while(strcmp(mess,"end"));
printf("\\nThread terminando...\\n");
pthread\_exit(NULL);
return 0;
}






int  open\_socket(int puerto, int id){
int on = 1;
int newsockfd, portno, clilen;
char buffer\[256\],test;
struct sockaddr\_in serv\_addr, cli\_addr;
int n;
id = socket(AF\_INET, SOCK\_STREAM, 0);
if (id < 0) printf("ERROR opening socket\\n");
bzero((char \*) &serv\_addr, sizeof(serv\_addr));
portno = (int) puerto;
serv\_addr.sin\_family = AF\_INET;
serv\_addr.sin\_addr.s\_addr = INADDR\_ANY;
serv\_addr.sin\_port = htons(portno);
setsockopt(id, SOL\_SOCKET, SO\_REUSEADDR, &on, sizeof( on)) ;
if (bind(id, (struct sockaddr \*) &serv\_addr, sizeof(serv\_addr)) < 0) printf("ERROR binding\\n");
listen(id,5);
clilen = sizeof(cli\_addr);
newsockfd = accept(id, (struct sockaddr \*) &cli\_addr, &clilen);
if (newsockfd < 0) printf("ERROR aceptando\\n");
return newsockfd;
}


void copyimage()
{
    int i,j;
    for(i=0;i<SIZEY;i++)
    for(j=0;j<SIZEX;j++)
        image\[i\*500+j\]=(char)w\[i\]\[j\];
    
}


int heat()
{
double u\[SIZEY\]\[SIZEX\];  
// El valor de temperatura del paso anterior, w, se copia al array u
// y a continuación se genera el nuevo valor de w, con los datos de u
// Esta forma de calcular la difusion no es correcta, pero es suficiente

//Varibles para medir tiempos
  double ctime;
  double ctime1;
  double ctime2;
  
  int i;
  int j;
  double mean;
  int success;
  
  ctime1 = cpu\_time ( );
  for ( i = 0; i < SIZEY; i++ ) 
    {
      for ( j = 0; j < SIZEX; j++ )
      {
        u\[i\]\[j\] = w\[i\]\[j\];
      }
    }
    for ( i = 1; i < SIZEY-1; i++ )
    {
      for ( j = 1; j < SIZEX - 1; j++ )
      {
        w\[i\]\[j\] = ( u\[i-1\]\[j\] + u\[i+1\]\[j\] + u\[i\]\[j-1\] + u\[i\]\[j+1\] ) / 4.0;

      }
    }
  return 0;
}




int main(int argc, char\*\* argv) {
    int puerto;
    pthread\_t mithread; /\* thread and attributes \*/
    int sock\_id, new\_sock\_id;
    if (argc < 2) {
        printf("Modo de empleo: servidor port\\n");
        exit(0);
    }
    puerto = atoi(argv\[1\]);
    printf("Abriendo puerto %d en espera de un cliente...\\n", puerto);
    new\_sock\_id = open\_socket(atoi(argv\[1\]), sock\_id);
    printf("Slave conectado.........\\n");
    pthread\_create(&mithread, NULL, funcion\_hilo, (void \*) (new\_sock\_id));
    printf("Thread iniciado \\n");
    iniheat();
    do {
        if (mess\[0\] == 0)printf(".");
        else printf(" %s\\n", mess);
        fflush(0);
        heat();
        copyimage();
        write(new\_sock\_id, image, SIZE);
        sleep(1);
    } while (strcmp(mess, "end"));
    printf("Esperando el fin del thread...\\n");
    pthread\_join(mithread, NULL);
    close(sock\_id);
    return (EXIT\_SUCCESS);
}