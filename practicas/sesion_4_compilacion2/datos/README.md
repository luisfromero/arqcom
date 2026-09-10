## Objetivo: Ver el resultado de una compilación de datos y entender al preprocesador

```c
#define VT 23

int a=23;
int b=-VT;
float c=-23.125;  // C1B90000h = 3250126848d
char d[4]="Ana";
unsigned char A=65;
unsigned char B=66;
unsigned char C=67;
unsigned char D=68;
char E[]="EFG";
unsigned short F=0, G=-1;
```


Compilar:

```mips-linux-gnu-gcc -c --save-temps datos.c```

cat datos.s
cat datos.i

Desensamblador:

```mips-linux-gnu-objdump --syms   -D datos.o```

(la opción --syms muestra la tabla de símbolos)

```assembly

00000000 <a>:
   0:   00000017        0x17

00000004 <b>:
   4:   ffffffe9        sdc3    $31,-23(ra)

00000008 <c>:
   8:   c1b90000        ll      t9,0(t5)

0000000c <d>:
   c:   416e6100        0x416e6100

00000010 <A>:
  10:           0x41424344

00000011 <B>:
  11:           addiu   v0,v0,3

00000012 <C>:
  12:           c0      0x1444546

00000013 <D>:
  13:           addiu   v0,a0,5

00000014 <E>:
  14:   45464700        0x45464700

00000018 <G>:
  18:   ffff0000        sdc3    $31,0(ra)
  1c:   00000000        nop
```