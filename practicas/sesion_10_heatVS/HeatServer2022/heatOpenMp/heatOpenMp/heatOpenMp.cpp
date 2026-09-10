/*
* File:   main.c
* Author: felipe
*
* Created on 1 de diciembre de 2015, 13:27
* Modificado el 30 de noviembre de 2018
*
* Servidor Heat. Se inicia en modo de espera de un cliente
*
* Cuando se establece la conexion, crea un thread que espera la llegada de comandos.
* El hilo principal envia paquetes de SIZE bytes, con los resultados del servicio
* El servicio ejecuta un paso del método explicito de resolucion por diferencias finitas
* de la eq de calor con un coeficiente de difusion arbitrario.
*/

#ifdef _WIN32

#define bzero(a, b) memset(a, 0x0, b)
#include "targetver.h"
#include <tchar.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")
#define HAVE_STRUCT_TIMESPEC
#include "pthread.h" 
#else
#include <pthread.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <omp.h>


#define SIZEX 500
#define SIZEY 500
#define MAXITER 500
#define SIZE SIZEX*SIZEY

// verbose:	Muestra mensajes
// timing: 	Mide tiempos
// standalone: 	Simula, pero no se comunica con la máquina externa
int  verbose = 0;
int  timing = 1;
int  standalone = 0;

#ifdef _WIN32
WSADATA wsaData;
#else
#define SOCKET int
#endif


//Variable global, compartida, con la temperatura 
double w[SIZEY][SIZEX];
double u[SIZEY][SIZEX];

char mess[100] = "";
char image[SIZE];
int nt, id;
int cnt;
int lim_inf, lim_sup;

// Variables globales (visible en todas las rutinas), pero de uso privatizado
#pragma omp threadprivate(id,lim_inf,lim_sup,u)

//Mide de forma precisa el tiempo de ejeq
double cpu_time(void)
{
#ifdef _WIN32
	FILETIME a, b, c, d;
	if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
		//  Returns total user time.
		//  Can be tweaked to include kernel times as well.
		return
			(double)(d.dwLowDateTime |
			((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
}
	else {
		//  Handle error
		return 0;
	}
#else
	return (double)clock() / (double)CLOCKS_PER_SEC;
#endif
}



//Establece una condicion de contorno Dirichlet
//          Frio
//Calor              Calor
//          Calor

//Establece una condicion de contorno Dirichlet
//          Frio
//Calor              Calor
//          Calor
int iniheat()
{
	int i, j;
	double mean;
	for (i = 1; i < SIZEY - 1; i++)
	{
		w[i][0] = 100.0;
	}
	for (i = 1; i < SIZEY - 1; i++)
	{
		w[i][SIZEX - 1] = 100.0;
	}
	for (j = 0; j < SIZEX; j++)
	{
		w[SIZEY - 1][j] = 100.0;
	}
	for (j = 0; j < SIZEX; j++)
	{
		w[0][j] = 0.0;
	}
	/*
	Calcula un valor interpolado en la zona interior
	*/
	mean = 0.0;
	for (i = 1; i < SIZEY - 1; i++)
	{
		mean = mean + w[i][0];
	}
	for (i = 1; i < SIZEY - 1; i++)
	{
		mean = mean + w[i][SIZEX - 1];
	}
	for (j = 0; j < SIZEX; j++)
	{
		mean = mean + w[SIZEY - 1][j];
	}
	for (j = 0; j < SIZEX; j++)
	{
		mean = mean + w[0][j];
	}
	mean = mean / (double)(2 * SIZEY + 2 * SIZEX - 4);
	/*
	Initialize the interior solution to the mean value.
	*/
	for (i = 1; i < SIZEY - 1; i++)
	{
		for (j = 1; j < SIZEX - 1; j++)
		{
			w[i][j] = mean;
		}
	}
	return 0;
}




#ifdef _WIN32
	// Initialize Winsock
SOCKET open_socket(int  puerto) {
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	char buffer[33];
	itoa(puerto, buffer, 10);
	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, buffer, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	SOCKET ClientSocket = INVALID_SOCKET;
	struct sockaddr cli_addr;
	// Accept a client socket
	int i = sizeof(cli_addr);
	ClientSocket = accept(ListenSocket, &cli_addr, &i);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	return ClientSocket;
}
#else
SOCKET open_socket(int  puerto) {
	int on = 1;
	int sockfd;
	int  portno, clilen;
	char buffer[256], test;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	int sockid = socket(AF_INET, SOCK_STREAM, 0);
	if (sockid < 0) 
		printf("ERROR opening socket\n");
	bzero((char *)&serv_addr, sizeof(serv_addr));
	portno = (int)puerto;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));
	if (bind(sockid, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR binding\n");
		exit(0);
	}
	listen(sockid, 5);
	clilen = sizeof(cli_addr);
	sockfd = accept(sockid, (struct sockaddr *) &cli_addr, &clilen);
	if (sockfd < 0) {
		printf("ERROR aceptando\n");
		exit(0);
	}

	return sockfd;
}
#endif
int heat()
{

	

	int i;
	int j;
	double mean;
	int success;
	int limi, lims;

#pragma omp barrier
	limi = (id == 0) ? 0 : lim_inf - 1;
	lims = (id == nt - 1) ? SIZEY : lim_sup + 1;
	for (i = limi; i < lims; i++)
	{
		for (j = 0; j < SIZEX; j++)
		{
			u[i][j] = w[i][j];
		}
	}
	limi = (id == 0) ? 1 : lim_inf;
	lims = (id == nt - 1) ? SIZEY - 1 : lim_sup;
#pragma omp barrier
	if (cnt == 0)printf("%d %d %d %d\n", id, cnt, limi, lims);
	for (i = limi; i < lims; i++)
	{
		for (j = 1; j < SIZEX - 1; j++)
		{
			w[i][j] = (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]) / 4.0;
		}
	}
	return 0;
}

void calentar(int x, int y, int t)
{
	int size = 5;
	int i, j;
	for (i = y - size; i <= y + size; i++)
		for (j = x - size; j <= x + size; j++)
		{
			if ((i>-1) && (i<SIZEY) && (j>-1) && (j<SIZEX))
				w[i][j] = t;
		}

}

void copyimage()
{
	int i, j;
	for (i = 0; i<SIZEY; i++)
		for (j = 0; j<SIZEX; j++)
			image[i*SIZEX + j] = (char)w[i][j];

}

int coordX, coordY, tempe;

void *funcion_hilo(void * psock_id) {
	int n = 0;
	SOCKET sock_id = *(SOCKET *)psock_id; //recuperamos parámetro a partir de puntero
	if (!standalone)
		do {
			//sleep(1);
			bzero(mess, 100);
			n = recv(sock_id, mess, 100,0);
			printf("\nEl thread ha recibido  %d bytes (%s)\n", n, mess);
			if (n < 0) perror("ERROR leyendo del socket");
			if (n>9)if (mess[0] == '$') {
				sscanf(mess, "$%03d%03d%03d", &coordX, &coordY, &tempe);

				if (verbose)printf("Recibidos %d y %d \n", coordX, coordY);
				//image[SIZEX*coordY+coordX]=128;
				//w[coordY][coordX]=250;
				calentar(coordX, coordY, tempe);
			}

		} while (strcmp(mess, "end"));
		printf("\nThread terminando...\n");
		pthread_exit(NULL);
		return 0;
}


int main(int argc, char** argv) {
	int puerto;
	omp_set_num_threads(6);
	pthread_t mithread; /* thread and attributes */
	SOCKET sock_id;
	sock_id = 0;
	if (!standalone) {
		if (argc < 2) {
			printf("Modo de empleo: servidor port\n");
			exit(0);
		}
		puerto = atoi(argv[1]);
		printf("Abriendo puerto %d en espera de un cliente...\n", puerto);
		int puerto = atoi(argv[1]);
		sock_id = open_socket(puerto);
		printf("Slave conectado.........\n");
		pthread_create(&mithread, NULL, funcion_hilo, (SOCKET *) &sock_id);
		printf("Thread iniciado \n");
	}
	iniheat();
	//--------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------
#pragma omp parallel 
	{
		cnt = 0;
		id = omp_get_thread_num();
		if (id == 0)nt = omp_get_num_threads();
#pragma omp barrier
		lim_inf = (SIZEY / nt)*id;
		lim_sup = (SIZEY / nt)*(id + 1) - 1;
		if (verbose)printf("Soy %d de %d, y calculo desde %d hasta %d \n", id, nt, lim_inf, lim_sup);
		double tiempos = 0;
		int condicionFin = 0;
		do {
			if (verbose) {
				if (mess[0] == 0)printf(".");
				else printf(" %s\n", mess);
				fflush(0);
			}

			double a, b;
			if (id == 0) a = cpu_time();
			heat();
			if (id == 0) {
				b = cpu_time();
				copyimage();
				tiempos += (b - a);
				if (!standalone)send(sock_id, image, SIZE,0);
			}
			cnt++;
			condicionFin = standalone ? (cnt<MAXITER) : !strcmp(mess, "end");
		} while (!condicionFin);

		if (timing)  if (id == 0)printf("Tiempo es %e )\n", tiempos / cnt);
	}
	//--------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------
	if (!standalone) {
		if (verbose)printf("Esperando el fin del thread de comunicación...\n");
#ifdef _WIN32
#else
#endif 
		pthread_join(mithread, NULL);
		if (verbose)printf("Cerrando el socket...\n");

#ifdef _WIN32
		closesocket(sock_id);
		WSACleanup();
#else
		close(sock_id);
#endif 

	}
	return (EXIT_SUCCESS);
} //end parallel section