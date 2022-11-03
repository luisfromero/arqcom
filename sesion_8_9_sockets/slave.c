#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

//main8.c

char mess[100]="";

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

void *funcion_hilo(void* new_sock_id) {
int n=0;
	do{
		sleep(1);
     		bzero(mess,100);
     		n = read(new_sock_id,mess,100);
		printf("\nEl thread ha recibido  %d bytes:",n);
     		if (n < 0) perror("ERROR leyendo del socket");
	}while(strcmp(mess,"end"));
printf("\nThread terminando...\n");
pthread_exit(NULL);
return 0;
	}

main(int argc, char *argv[]) {
int i;
pthread_t mithread;    /* thread and attributes */
int sock_id, new_sock_id;
if(argc<2){printf("Modo de empleo: slave port\n");exit(0);}
	printf("Abriendo puerto...\n");
	new_sock_id=open_socket(atoi(argv[1]),sock_id);
	printf("Slave conectado.........\n");
	pthread_create(&mithread,NULL, funcion_hilo,(void *) (new_sock_id));
	printf("Thread iniciado \n");
	do{
		if(mess[0]==0)printf(".");
		else printf(" %s\n",mess);
		fflush(0);
		sleep(1);
	}while(strcmp(mess,"end"));
printf("Esperando el fin del thread...\n");
pthread_join(mithread,NULL);
close(sock_id);
}

