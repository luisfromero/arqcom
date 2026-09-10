#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

char mess[100];

/**
 * Función que se conecta a a un socket
 * @param int puerto Puerto al que se conecta
 * @param char* host Máquina a la que se conecta
 * @author Yo
 * @returns int Si se ha conectado o no
*/
int open_socket_client(int puerto, char *host)
{
int fd;
struct hostent *he;
struct sockaddr_in server;

if((he=gethostbyname(host))==NULL){
	printf("Error gethosbyname\n" ); exit(-1); }
if( ( fd=socket(AF_INET, SOCK_STREAM,0) ) ==-1) {
	printf("Error creando  socket\n"); exit(-1); }

server.sin_family=AF_INET;
server.sin_port=htons(puerto);
server.sin_addr=*((struct in_addr*) he->h_addr);
bzero(&(server.sin_zero),0);

if(connect(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1){
	printf("Error al conectar. Socket no disponible\n"); exit(-1); } 
return fd;
}

main(int argc, char *argv[]) {
int i;
int new_sock_id;
if(argc<3){printf("Modo de empleo: master IP port\n");exit(0);}
	printf("Abriendo puerto %d en %s..\n",atoi(argv[2]),argv[1]);
	new_sock_id=open_socket_client(atoi(argv[2]),argv[1]);
	printf("Conectado. Escriba palabras y pulse INTRO. Para terminar, escriba \"end\"\n");
	do{
		scanf("%s",mess);
		write(new_sock_id, mess, 1+strlen(mess));
	}while(strcmp(mess,"end"));
printf("Cerrando socket...\n");
close(new_sock_id);
}

