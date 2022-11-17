#ifndef _XFUN_H
#define _XFUN_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

extern enum _ed { ABIERTO , CERRADO } estadoDisp ;

void iniciaDisp( void ) ;
void cierraDisp( void ) ;
void pinta( void ) ;
void ponImagen( char * imag , int ancho , int alto ) ;

#endif
