/* 
 * File:   main.c
 * Author: felipe
 *
 * Created on 1 de diciembre de 2015, 13:27
 * Modificado el 12 de noviembre de 2020
 * 
 * Servidor Heat. Se inicia en modo de espera de un cliente
 * 
 * Cuando se establece la conexion, crea un thread que espera la llegada de comandos.
 * El hilo principal envia paquetes de SIZE bytes, con los resultados del servicio 
 * El servicio ejecuta un paso del método explicito de resolucion por diferencias finitas
 * de la eq de calor con un coeficiente de difusion arbitrario.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> 

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
//OpenMP Será necesario más adelante
#include <omp.h>
#include <stdint.h>
#include  <unistd.h>
#define SIZEX 500
#define SIZEY 500
#define SIZE SIZEX*SIZEY 
//Variable global, compartida, con la temperatura 
double w[SIZEY][SIZEX];  //Espacio para guardar los 250000 valores de temperatura (double)
char mess[100]="";
//Espacio para guardar la informacion de temperatura compacta (1 byte por punto, 250000 elementos)
char image[SIZE];

//Mide de forma precisa el tiempo de ejeq
double cpu_time ( void )
  {
    return ( double ) clock ( ) / ( double ) CLOCKS_PER_SEC;
  }





void calienta(int x,int y,int tempe)
{
int r=10;
for(int i=-r;i<=r;i++)
for(int j=-r;j<=r;j++)
w[max(0,y+i)][max(0,x+j)]=tempe;
}



int coordX,coordY,tempe;  //Datos que se reciben del cliente por un thread separado

void *funcion_hilo(void* new_sock_id) {
int n=0;

do{
//sleep(1);
     bzero(mess,100);
     n = read((intptr_t)new_sock_id,mess,100);
     printf("\nEl thread ha recibido  %d bytes (%s)\n",n,mess);
     if (n < 0) perror("ERROR leyendo del socket");
                        if(n>9)if(mess[0]=='$'){
                            sscanf(mess,"$%03d%03d%03d",&coordX,&coordY,&tempe);
                            printf("\nHas recibido %dº en las coordenadas %d,%d",tempe,coordX,coordY);
                            w[coordY][coordX]=tempe;
                            //El cliente cambia la temperatura de un determinado punto
                            //El alumno puede mejorar esta operacion, creando una funcion void calienta(int x, int y, int z);
                            //calienta(coordX,coordY,tempe); //No solo actualiza w en un punto. En los vecinos tambien
                        }
                        
}while(strcmp(mess,"end"));
printf("\nThread terminando...\n");
pthread_exit(NULL);
return 0;
}






int  open_socket(int puerto, int id){
int on = 1;
int newsockfd, portno, clilen;
char buffer[256],test;
struct sockaddr_in serv_addr, cli_addr;
int n;
id = socket(AF_INET, SOCK_STREAM, 0);
if (id < 0) printf("ERROR opening socket\n");
bzero((char *) &serv_addr, sizeof(serv_addr));
portno = (int) puerto;
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port = htons(portno);
setsockopt(id, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on)) ;
if (bind(id, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) printf("ERROR binding\n");
listen(id,5);
clilen = sizeof(cli_addr);
newsockfd = accept(id, (struct sockaddr *) &cli_addr, &clilen);
if (newsockfd < 0) printf("ERROR aceptando\n");
return newsockfd;
}


void copyimage()
{
    int i,j;
    for(i=0;i<SIZEY;i++)
    for(j=0;j<SIZEX;j++)
        image[i*500+j]=(char)w[i][j];
    
}

//Establece una condicion de contorno Dirichlet
//          Frio
//Calor              Calor
//          Calor

//Establece una condicion de contorno Dirichlet
//          Templado
//Fresco              Frío
//          Calor
int iniheat()
{
	int i, j;
	double mean;
	for (i = 1; i < SIZEY - 1; i++)
		w[i][0] = 200.0;
	for (i = 1; i < SIZEY - 1; i++)
		w[i][SIZEX - 1] = 100.0;
	for (j = 0; j < SIZEX; j++)
		w[SIZEY - 1][j] = 255.0;
	for (j = 0; j < SIZEX; j++)
		w[0][j] = 40.0;
	
	//Calcula un valor interpolado en la zona interior
	mean = 0.0;
	for (i = 1; i < SIZEY - 1; i++)
		mean = mean + w[i][0];
	for (i = 1; i < SIZEY - 1; i++)
		mean = mean + w[i][SIZEX - 1];
	for (j = 0; j < SIZEX; j++)
		mean = mean + w[SIZEY - 1][j];
	for (j = 0; j < SIZEX; j++)
		mean = mean + w[0][j];
	mean = mean / (double)(2 * SIZEY + 2 * SIZEX - 4);
	
	/*
	Initialize the inner solution:
	*/
	for (i = 1; i < SIZEY - 1; i++)
		for (j = 1; j < SIZEX - 1; j++)
			w[i][j] = mean;
			w[i][j] = (w[0][j] + w[SIZEX - 1][j] + w[i][0] + w[i][SIZEY - 1]) / 4;
	return 0;
}


int heat()
{
double u[SIZEY][SIZEX];  
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
  
  ctime1 = cpu_time ( );
  for ( i = 0; i < SIZEY; i++ ) 
    {
      for ( j = 0; j < SIZEX; j++ )
      {
        u[i][j] = w[i][j];
      }
    }
    for ( i = 1; i < SIZEY-1; i++ )
    {
      for ( j = 1; j < SIZEX - 1; j++ )
      {
        w[i][j] = ( u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1] ) / 4.0;

      }
    }
  return 0;
}



int main(int argc, char** argv) {
    int puerto;
    pthread_t mithread; /* thread and attributes */
    int sock_id, new_sock_id;
    if (argc < 2) {
        printf("Modo de empleo: %s puerto\n",argv[0]);
        exit(0);
    }
    puerto = atoi(argv[1]);
    printf("Abriendo puerto %d en espera de un cliente...\n", puerto);
    new_sock_id = open_socket(atoi(argv[1]), sock_id);
    printf("Slave conectado.........\n");
    pthread_create(&mithread, NULL, funcion_hilo, (void *)(intptr_t) (new_sock_id));
    printf("Thread iniciado \n");
    iniheat();
    do {
        if (mess[0] == 0)printf(".");
        else printf(" %s\n", mess);
        fflush(0);
        heat();
        copyimage();
        write(new_sock_id, image, SIZE);
        sleep(0.1);
    } while (strcmp(mess, "end"));
    printf("Esperando el fin del thread...\n");
    pthread_join(mithread, NULL);
    close(sock_id);
    return (EXIT_SUCCESS);
}