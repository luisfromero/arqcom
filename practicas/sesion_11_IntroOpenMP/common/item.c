#include "../common/x.h"  
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <stdio.h>
#include "../common/shark.h"
#ifdef PTHREAD
#include <pthread.h>
#endif

float frandom()
{
float i;
i=((float) random())/0x80000000;
return(i);
}


int nherr,nshark;
int neig[8][2];
unsigned short int hbreed,sbreed,starvetime;
#ifdef PTHREAD
extern pthread_mutex_t lock;
#endif

unsigned char randarray[RNDARRSZE];
unsigned char rand8(unsigned int *cnt)
{
unsigned char c= randarray[((*cnt)++)%RNDARRSZE];
return c;
}




/************************************************************************
El mundo es de dimx*dimy de tamano y en cada celda existe un entero cuyos
	       16 bits tienen el siguiente signif: pevx xxaa aaar rrrr:

pe      tipo de pez (10: peces; 01: escualos; 00: vacio);
v	El animal ya se ha movido en este paso;
aaaaa	Tiempo que lleva ayunando si es un tiburon;
rrrrr	Tiempo que ha transcurrido desde la ultima reproduccion;
************************************************************************/


int item_(unsigned short int m[dimy][dimx], int *pi,int *pj,int *pstpp, thread_data *localdata)
{
int i,j,stpp;
int ni,nj,n,l,ti,tj,tn,k2;
short int p,k;
i=*pi;
j=*pj;
stpp=*pstpp;
/* vecinos:       0 1 2
		  7 p 3  Cantidad que se suma para ir al vecino
		  6 5 4  */


if((p=m[i][j])!=0){

if(((p&(0x2000))>>13)==stpp)
	{ 	/* Si no se ha movido en esta iteracion */
	p^=0x2000;   //cambia la paridad del individuo

	p++;		// aumenta su edad
	l=rand8(&RANDCNT);
	if(0x8000&p)	{
			for(k=0;k<8;k++)	
					{  /* si es un pez, recorre el vecindario para buscar hueco*/
					n=(k+l)%8;
					ni=mod(i+neig[n][1],dimy);
					nj=mod(j+neig[n][0],dimx);
					ni=neigh_i(n);
					nj=neigh_j(n);
					if(!m[ni][nj])break;
					} /*encuentra hueco*/
			if((0x001f&p)==hbreed)
					{          /* toca reproduccion*/
					m[i][j]=(p&0xa000);
				  	if(k!=8)
						{
						m[ni][nj]=(p&0xa000);
           					//pthread_mutex_lock(&lock) ;
						//herr++;
						NHERR++;
           					//pthread_mutex_unlock(&lock) ;
						}
					}
			else		
					{     
					if(k!=8)
						{
						m[ni][nj]=p;   /*se mueve si hay hueco*/
						m[i][j]=0;
						}
				  	else m[i][j]=p;
					}
		    	return 1;
		    	}


	else   	    	{	
			p+=0x0020; // tiburon por colocar un dia mas viejo
			if(((0x03e0&p)>>5)==starvetime)
				{
				m[i][j]=0;   /* inanicion? */
				NSHARK--;
				return 2;
				}
			for(k=0;k<8;k++)
				{   /* busca hueco vecino*/
				n=(k+l)%8;
				ni=mod(i+neig[n][1],dimy);
				nj=mod(j+neig[n][0],dimx);
					ni=neigh_i(n);
					nj=neigh_j(n);
				if(!m[ni][nj])break;
				}
			for(k2=0;k2<8;k2++)
				{   /* busca un pez vecino */
				tn=(k2+l)%8;
				ti=mod(i+neig[tn][1],dimy);
				tj=mod(j+neig[tn][0],dimx);
					ti=neigh_i(tn);
					tj=neigh_j(tn);
				if(m[ti][tj]&0x8000)	 // en el vecindario hay un pez
					{
					ni=ti;     /* si ese pez esta all� antes de este paso, se lo come*/
				 	nj=tj;
					n=tn;  //mi destino es pez en lugar de hueco
					p=p&0xfc1f; // el tiburon a colocar tiene el estomago lleno
					NHERR--;
					break;
					}
				}

			if((0x001f&p)==sbreed)
				{
				m[i][j]=(p&0xffe0);  /* si toca repro, */
				if((k!=8)||(k2!=8))
					{
					m[ni][nj]=(p&0xffe0);



						NSHARK++;
					}
				}
			else	{	
				if((k!=8)||(k2!=8)){
					   m[ni][nj]=p;
					   m[i][j]=0;
					   }
				else m[i][j]=p;//no puedo moverme
				}

		    	return 2;//tibu processed
			}
	}
return -1; //ya procesado
}
return 0;  //No hay nada
}

/*********************/
int initialize_(unsigned short int m[dimy][dimx], int  usex, int  askd, thread_data *tdata)
{
int i,x,y;
short int j,k;
char d,dato[256];
if(usex)iniciaDisp();
thread_data *localdata=tdata;
for(i=0;i<NT;i++)
	{
	NSHARK=0;
	NHERR=0;
	RANDCNT=TID*100000;
	localdata++;
	}
for(i=0;i<RNDARRSZE;i++)
	randarray[i]=(char)8*frandom();
//printf("ok rand\n");


if(askd){
printf("\n\nFichero o dato (f,d)? ");
fgets(dato,2,stdin);
d=dato[0];
for(i=0;i<dimy;i++)
	for(j=0;j<dimx;j++)
	m[i][j]=0;

//printf("test%s\n");
if ((d==13)||(d==102))
	1;
	/*savldmatriz(0);*/
else	{
	printf("\n\n Numero de peces ? ");
	fgets(dato,5,stdin);
	sscanf(dato,"%d",&nherr);if(nherr==0)nherr=defnh;
	printf("\n Numero de escualos ? ");
	fgets(dato,5,stdin);
	sscanf(dato,"%d",&nshark);if(nshark==0)nshark=defns;
	printf("\n T repro de peces ?(0-31) ");
	fgets(dato,3,stdin);
	sscanf(dato,"%d",&hbreed);if(hbreed==0)hbreed=defhb;
	printf("\n T repro de escualos ?(0-31) ");
	fgets(dato,3,stdin);
	sscanf(dato,"%d",&sbreed);if(sbreed==0)sbreed=defsb;
	printf("\n T ayuno de escualos ?(0-31) ");
	fgets(dato,3,stdin);
	sscanf(dato,"%d",&starvetime);if(starvetime==0)starvetime=defst;
	}
}
else
{
float escala=dimx*dimy/120000.0;
nherr=defnh*escala;
nshark=defns*escala;
hbreed=defhb;
sbreed=defsb;
starvetime=defst;
}

/*
srand(time(NULL));
*/
neig[0][0]=-1; neig[0][1]=-1; neig[1][0]=0; neig[1][1]=-1;
neig[2][0]=1; neig[2][1]=-1;neig[3][0]=1; neig[3][1]=0;
neig[4][0]=1;neig[4][1]=1;neig[5][0]=0;neig[5][1]=1;
neig[6][0]=-1;neig[6][1]=1;neig[7][0]=-1;neig[7][1]=0;

/*
 * Dibujar recuadro
*/
/* Si es fichero, lo dibuja y marca los peces con paridad de vida par */

if((d==102)||(d==13)){
	for(i=0;i<dimy;i++)for(j=0;j<dimx;j++){
		if(m[i][j]&0x8000){
			m[i][j]|=0x2000;}
		if(m[i][j]&0x4000){
			m[i][j]|=0x2000;}}
		}
else{

	for(i=1;i<=nherr;i++){  do{
			x=dimx*frandom();
			y=dimy*frandom();
			}while(m[y][x]);
			k=hbreed*frandom();
			m[y][x]=k+0xa000;

			}
j=sbreed*frandom();

for(i=1;i<=nshark;i++){ do{
			x=dimx*frandom();
			y=dimy*frandom();
			}while(m[y][x]);
			k=((short int) ((starvetime-1)*frandom()) )<<5;
			m[y][x]=k+0x6000;
			j=(j+1)%sbreed;
			m[y][x]+=j;
			}


   }
return 0;
}

/*
 * Funciones para manejar una ventana X11 donde mostrar un 
 * grafico 
 */


/* variables globales ******************************************************/

enum _ed estadoDisp = CERRADO ; /* estado de la ventana       */
Display * pantalla  = NULL    ; /* manejador de la pantalla   */
Window    ventana             ; /* ventana                    */
XSizeHints sh                 ;
XGCValues  cgv                ;
GC         cg                 ;
XEvent     Evento             ; 
XImage   * imagen   = NULL    ;
Colormap   cmp                ; 
XColor     color              ;
unsigned long RGB[64]; /* Para cojer los pixeles del los colores*/



/* Funciones ***************************************************************/

void iniciaDisp() {
int argc=0;
char *argv[0];//=NULL;

   if( estadoDisp == ABIERTO ) return ;

   /* Abrimos la pantalla donde queremos mostrar la imagen */

   if( (pantalla = XOpenDisplay( NULL ) ) == NULL ) return ;

   sh.x = 10 ; sh.y = 10 ;
   sh.width = dimx ; sh.height = dimy ;

   /* Ahora se crea la ventana *************************************/

   ventana = XCreateSimpleWindow( pantalla , 
                        RootWindow(pantalla , 0 ) ,
                        sh.x , sh.y , /* Esquina superior derecha */
                        sh.width    , /* Ancho y alto             */
                        sh.height   , 
                        4           , /* Ancho del borde          */
                        120         , /* color del borde ???      */
                        WhitePixel( pantalla, 0 ) /* Color del fondo */ ) ;

   /* y se le asignan propiedades **********************************/

   sh.flags  = PPosition | PSize | PMinSize;


   XSetStandardProperties( pantalla , ventana, "Vision" , "Vision" , None , argv , argc , &sh ) ;
        
   /* se lee el contexto grafico ***********************************/

   cgv.foreground=BlackPixel( pantalla, 0 );
   cgv.background=WhitePixel( pantalla, 0 );

   cg = XCreateGC( pantalla , ventana , GCForeground , &cgv ) ;

   /* conectamos la aplicacion a la cola de eventos ****************/

   XSelectInput( pantalla, ventana,  KeyPressMask | KeyReleaseMask | 
                 ExposureMask | ButtonPressMask | KeymapStateMask ) ;

   XMapWindow( pantalla, ventana ) ;
   XFlush( pantalla ) ;
   
   /* y esperamos que la pantalla se vea */

   do {
     XNextEvent( pantalla , &Evento ) ;
   }while( Evento.type != Expose ) ;

   imagen = XGetImage( pantalla, ventana , 0 ,  0 , 
                       sh.width, sh.height, AllPlanes, ZPixmap ) ; 
   

   /* ahora se asigna la tabla de 64 tonos de grises */

   cmp = DefaultColormap( pantalla, 0 ) ;

   { int f ; char x[ 10 ] ;
     for( f = 0 ; f < 256 ; f += 4 ) {
        sprintf( x , "#%02x%02x%02x", 255-f , 254 , 255-f ) ;
if(f==128)sprintf(x,"#%02x%02x%02x", 255 , 0 , 0 ) ;
        if( XParseColor( pantalla, cmp, x , &color ) ) {
           if( XAllocColor(  pantalla, cmp, &color ) ) {
               RGB[ f>>2 ] = color.pixel ;
           } else printf( "\nXAllocColor %d" , f>>2 ) ;
        } else printf( "\nXParseColor %d" , f>>2 ) ;
     }   



   }
   XSetWindowColormap( pantalla, ventana, cmp ) ;
}

int finalize_(  int  usex) {

 //printf("Sharks:%d Herrs:%d\n",nshark,nherr);
    if(!usex)return 0;
    if( imagen != NULL ) { XDestroyImage( imagen ) ; imagen = NULL ; }
  
    if( pantalla != NULL ) { 
          XDestroyWindow( pantalla , ventana ) ;
          XCloseDisplay( pantalla ) ; pantalla = NULL ; 
    }
    
    estadoDisp = CERRADO ; 
    return 1;
}

void pinta( void ) {
   
    int f ; 
    for( f = 0 ; f < 64 ; f++ ) {

    XSetForeground( pantalla, cg, RGB[ f ] ) ;
    XFillRectangle( pantalla, ventana, cg, 10, 10, 60, 60 ) ;
    /*XClearArea(pantalla, ventana, 0, 0, 100, 100, 0 ) ; */
    XFlush( pantalla ) ;
    printf( "\n%d" , f ) ;
    getchar( ) ;
    }
}


void putImage( char * imag , int ancho , int alto ) {
    int x , y ; long por = 0 ; 
    if( imagen == NULL )
          imagen = XGetImage( pantalla, ventana, 0,0, 
                               ancho, alto, AllPlanes,ZPixmap ) ;
    for( y = 0 ; y < alto ; y++ ) 
        for( x = 0 ; x < ancho ; x++ ) 
           XPutPixel( imagen, x, y, RGB[ (unsigned ) imag[ por++ ] ] ) ;
     XPutImage( pantalla, ventana, cg, imagen, 0, 0, 0, 0, ancho, alto );
     XFlush( pantalla ) ;
     XDestroyImage( imagen ) ; imagen = NULL ;  
}


int drawWorld_(unsigned short int m[dimy][dimx],int  usex)
{
short int p;
int i,j;
unsigned long col[3];
if(!usex)return 0;
col[0]=RGB[1];
col[1]=RGB[32];
col[2]=RGB[63];
for(i=0;i<dimy;i++)
  for(j=0;j<dimx;j++){
    p=(m[i][j]>>14)&0x0003;
    XPutPixel(imagen,j,i,col[p]);
    }
    XPutImage( pantalla, ventana, cg, imagen, 0, 0, 0, 0, dimx,dimy );
    XFlush(pantalla);
return 0;  
}






















