                            CASIUM - Simuladores de arquitectura de computadores    $("#menu-toggle").click(function(e) { e.preventDefault(); $("#wrapper").toggleClass("toggled"); }); 

Práctica 1. Conexión a una máquina remota y primeros pasos de compilación.

En esta práctica nos vamos a conectar a una máquina denominada aloe (150.214.109.24). 

  

  

*   Nombre: [aloe.ac.uma.es](http://aloe.ac.uma.es/) (150.214.109.24)
*   Dos procesadores Xeon Haswell E5-2698 V3 de 2.3GHz de 16 cores y 40MB L3 (total 32 cores)
*   Placa base con 16 bancos de 16GB cada uno (total 256GB de RAM)
*   Slots: 4 PCI-e 3.0 x16, 2 PCI-e 3.0 x8 y 1 PCI-e x4
*   5TB de disco duro
*   2 GPUs NVIDIA Tesla K40c. 
*   2 GPUs de la serie Maxwell. 
*   1 GPU Pascal Titan X

  

![](img/arqcom/intel-chip-v2.png)  

  

En esta máquina, con sistema operativo Ubuntu 18.04.4 LTS, a la que nos conectaremos con el software **mobaxterm**, que instalaremos desde la siguiente url:

  

[https://mobaxterm.mobatek.net/download-home-edition.html](https://mobaxterm.mobatek.net/download-home-edition.html)  

  

Este software contiene tres elementos esenciales para la conexión a sistemas remotos:

  

*   Es un terminal remoto (el nombre procede de los antiguos computadores, en los años 50 y 60, en los que una misma máquina tenía distintos pares teclado-monitor para los distintos usuarios. Los primeros terminales sólo visualizaban texto, hasta que empezaron a aparecer los primeros terminales gráficos ("x-term")  http://es.wikipedia.org/wiki/Sistema\_de\_ventanas\_X (X11)![](img/arqcom/unnamed.jpg)
*   Es un "servidor X11". Permite que aplicaciones gráficas que se ejecutan en una máquina remota sean mostradas en la pantalla del ordenador local (en el que está sentado el alumno delante). Más información en

  

Usuarios de la máquina  

  

El usuario: ac001    Tiene la clave: 913616   
El usuario: ac002    Tiene la clave: 931382   
El usuario: ac003    Tiene la clave: 940323   
El usuario: ac004    Tiene la clave: 947792   
El usuario: ac005    Tiene la clave: 955388   
El usuario: ac006    Tiene la clave: 963752   
El usuario: ac007    Tiene la clave: 971308   
El usuario: ac008    Tiene la clave: 979373   
El usuario: ac009    Tiene la clave: 987304   
El usuario: ac010    Tiene la clave: 994859   
El usuario: ac011    Tiene la clave: 001570   
El usuario: ac012    Tiene la clave: 005152   
El usuario: ac013    Tiene la clave: 012929   
El usuario: ac014    Tiene la clave: 018218   
El usuario: ac015    Tiene la clave: 024010   
El usuario: ac016    Tiene la clave: 029091   
El usuario: ac017    Tiene la clave: 034177   
El usuario: ac018    Tiene la clave: 039466   
El usuario: ac019    Tiene la clave: 045137   
El usuario: ac020    Tiene la clave: 050405  

  

  

Acceso a la máquina

  

*   El alumno aprende a conectarse a la máquina utilizando el software **mobaxterm**, habiendo entendido algunos conceptos muy básicos del protocolo TCP/IP 
    *   Se conecta utilizando una máquina intermedia (login.ac.uma.es) que establece un tunel para acceder a la máquina final (aloe).

  

  

Primeros comandos

  

*   El alumno aprende algunos comandos básicos, como who, whoami, pwd, echo, man, tty, cat, lshw
*   El alumno entiende los conceptos más básicos de entrada y salida, para cauces y redirecciones (more)
*   http://es.wikipedia.org/wiki/Unix#.C3.93rdenes\_cl.C3.A1sicas\_de\_UNIX  
    

  

Acceso a directorios

  

*   Tras aprender algunas nociones básicas del sistema de archivos de linux...
*   El alumno practica con los comandos que le permiten crear directorios, cambiar directorios, conocer el directorio por defecto, etc., utilizando los comandos: cd, mkdir, pwd. Aprende el significado de las carpetas y símbolos . (directorio de trabajo); .. (directorio superior); / (directorio  raíz); /dir1/dir2... (directorio absoluto) dir1/dir2... (directorio relativo al actual)
*   Conceptos básicos de seguridad de archivos y directorios
*   El alumno practica la copia, movimiento y borrado de archivos y carpetas (comandos cp, mv, rm); la creación de un archivo vacío (touch), la edición de texto en modo gráfico (gedit), el volcado en  pantalla del contenido de un archivo (cat)

Compilación básica

  

El programa que utilizaremos para compilar es el [GNU C compiler](https://gcc.gnu.org/), cuyo front-end ejecutable es **gcc**. Con **gcc** podemos convertir cualquier archivo en lenguaje C o C++, en su equivalente versión binaria, usando el archivo como argumento (flujo de entrada) y obteniendo el "objeto binario" en el flujo de salida (resultado)

  

Una instrucción de alto nivel suele necesitar muchas instrucciones máquina para realizarse. Por ello es necesario un proceso para generar código máquina a partir de un fichero fuente escrito en lenguaje de alto nivel. Por ese motivo, el proceso de compilación se realiza en tres fases: 

  

**1ª Fase:** El código fuente de alto nivel es convertido en un código intermedio ensamblador equivalente. El programa traductor se denomina compilador (los compiladores detectan y arreglan también los pequeños defectos de programación). 

  

**2ª Fase:** El código ensamblador generado es convertido en código en lenguaje máquina objeto. 

  

![](img/arqcom/Clipboard03.jpg)  

  

La compilación en realidad, finaliza con la creación del archivo objeto. Utilice el comando **gcc -c fuente.c** (la opción c significa "sólo compilar")

**3ª Fase:** El código objeto generado por el compilador no es directamente ejecutable, ya que no tiene "resueltas” todas las rutinas y variables, es decir, es posible que no estén definidas todas las variables o rutinas invocadas en este código. Para ello es necesario enlazar todos los ficheros binarios que incluyen todas las rutinas en un solo programa, como por ejemplo _programa.exe, comando.com,_ o _a.out_. El programa que une todos los códigos objeto es un programa enlazador (_link_).   

  

El resultado final será un programa ejecutable. No es necesario por tanto que todas las rutinas estén en el mismo fichero. Se pueden compilar por separado. 

  

**Ejemplo 1**: Tenemos este programa:   

  

![](img/arqcom/Clipboard02.jpg)  

  

El programa compilado y convertido a objeto no es aún ejecutable porque no contiene ninguna rutina que indique como realizar la operación print ("que resuelva print”), por eso se requiere su enlazado con otro fichero que lo resuelva. En cambio, la función o rutina fact() sí está resuelta. Es  necesario enlazarlo con un objeto (o librería) que defina la función print. Una librería es un conjunto de objetos.

  

El comando gcc sin opción adicional compila y enlaza:

  

gcc main.c  (compila a main.o, y si todo está resuelto, genera el ejecutable, normalmente a.out)

**Ejemplo 2**: Ahora, el programador escribe el código fuente de la rutina principal. Además, se

encarga de escribir en otros ficheros el código fuente de las rutinas 1 y 2 que se llaman desde el

principal (es decir, las "resuelve” en otros ficheros fuente):

  

![](img/arqcom/Clipboard01.jpg)  

La compilación de varios archivos y su posterior enlazado sólo requiere un comando:

  

gcc main.c  rutina1.c rutina2.c  (compila tres archivos. los enlaza, y si todo está resuelto, genera el ejecutable, normalmente **a.out**)

  

Como se ve, el enlazador no sólo garantiza que las funciones estén resueltas, sino que además las direcciones de las llamadas a rutinas se correspondan con las verdaderas direcciones. Además, realiza muchas otras operaciones sobre el código final que no es necesario conocer (por ejemplo, añade encabezados que permiten la ejecución en el sistema operativo actual). 

  

**Práctica del alumno**

Para probar estos conceptos el alumno compila los archivos

  

code00.c:

int main()

{

int x=3;

}

  

code01.c:

int main()

{

int x=3;

x = func();

}

  

code02.c:

int main()

{

int x=3;

x = func();

}

  

code04.c:

int a=23;

float f=23.5;

  

##### Compilación nativa y cruzada

Se entiende por compilación nativa al proceso de compilación que se desarrolla en un tipo de arquitectura y cuyos resultados binarios se generan para la misma arquitectura, mientras que en la compilación cruzada, la arquitectura en la que se compila no coincide con la de los resultados. 

  

En las prácticas, vamos a compilar en una arquitectura intel x64 (Xeón), mientras que los resultados se generan para la arquitectura MIPS.

  

**Compilación nativa** 

  

Copia los archivos fuentes en lenguaje C de la carpeta /tmp/arqcom/paso0 a la carpeta ./paso0 y compílalos utilizando el compilador gratuito GNU gcc. 

  

Ejemplo: gcc main0.c Como el archivo fuente es "autosuficiente”, el compilador ha ejecutado los tres pasos descritos arriba sin problemas. Los archivos intermedios \*.o y \*.s han sido eliminados y sólo se ha creado el ejecutable a.out. Se puede ejecutar, tecleando ./a.out (intro). Como la compilación genera un código que se puede ejecutar en la misma arquitectura que el compilador, se dice que es nativa. Ejecuta ahora la compilación gcc -S main0.c (sólo paso 1) y gcc -c main0.c (pasos 1 y 2). Visualice los archivos intermedios con los comandos cat main0.s y hexdump main0.o. Visualice ahora el contenido del objeto, con el "desensamblador” objdump -d main0.o 

  

**Compilación cruzada**

  

Repita los mismos pasos, utilizando es este caso el ensamblador cruzado mips-linux-gnu-gcc y el desensamblador (mips-linux-gnu-objdump -d main0.o). 

Cuando se compila un código para una arquitectura diferente a la del programa compilador, el proceso se denomina compilación cruzada. Es muy útil cuando hay dificultad para disponer de un compilador para ejecutar en la arquitectura destino.  

Práctica ?. Planificación. Salto retardado y Loop Unrolling

Edita y compila los siguientes archivos, con las opciones de compilación:  
  
**datos.c**  

char b\[6\]="enigma";
float f=23.25;
double s=23.25;
**loop.c**
double \*a;
extern double s;

  
**loop.c**  

void loop()
{
        for(int i=0;i<2000;i++)
        {
                a\[i\]=a\[i\]+s;
        }
}

  
**main.c**

void loop();
int main()
{
        loop();
}

  
Observe que puede ver el código ensamblador mediante dos opciones diferentes:

1.  Mediante la opción que inhabilita el borrado del archivo intermedio (.s) durante el proceso de compilación
2.  Mediante el desensamblado del código binario con _objdump -d binaryfile_. Esta opción ofrece menos información en general.

  
**Prueba 1. Optimización cero. Almacenamiento de archivos intermedios.**  
mips-linux-gnu-gcc -c -O0 -save-temps datos.c loop.c main.c  
**Prueba 2. Optimización 5 (muy alta). Almacenamiento de archivos intermedios.**  
mips-linux-gnu-gcc -c -O5 -save-temps datos.c loop.c main.c  
**Prueba 3. Desenrollamiento de lazos.**  
mips-linux-gnu-gcc -c -Ofast -funroll-all-loop -save-temps datos.c loop.c main.c  

Práctica 5. Threading

#### Los hilos o threads

  

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

  

##### Los hilos POSIX (pthreads)

  

El Sistema Operativo Unix/Linux ya dispone de funciones (fork o clone) que permiten que un  
proceso se replique. A partir del punto en el que se llama a la función, el sistema operativo los  
considera como dos tareas casi independientes (por ejemplo, en el reparto de tiempo de CPU  
mediante time slicing). Sin embargo, la creación de hilos, la comunicación entre ellos y la propia compartición de memoria es más compleja  
cuando se implementa mediante "llamadas al sistema" (traps, a bajo nivel), por lo que lo más habitual es utilizar  
librerías de más alto nivel. Entre ellas, las más conocidas son las librerías POSIX-threads (o  
pthreads) y OpenMP.  

  

##### Creación de un Pthread 

  

El siguiente código muestra la creación de un hilo con la librería pthread. Averigüe cuál es el  
proceso principal y la tarea del hilo creado. Averigüe para qué sirve la función _join_. A continuación,  
compílelo (añada la opción -lpthread) y ejecútelo.

  

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

//Only a thread

void \*thread\_func(void\* argv)
{
sleep(5);
printf("Hello world\\n");
}

main(int argc, char \*argv\[\])
{
int i;
pthread\_t mythread,my2;    /\* thread and attributes \*/
pthread\_create(&mythread,NULL, thread\_func,NULL);
for(i=9;i>=0;i--){
        printf("%d\\n",i);
        sleep(1);
        }
pthread\_join(mythread,NULL);
}

   

  

Utilice el comando 

**gcc -o mainX.exe mainX.c -pthread**

Descargue el resto de archivos para esta práctica, [de éste enlace](http://casium.uma.es/descargas/codigosThreads.zip).

  

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