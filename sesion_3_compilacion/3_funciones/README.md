Objetivo: compilar una función simple con distintos tipos de variables:

``` 
extern int x;     
// La dirección del área global se guarda en r28
// El símbolo es el desplazamiento
int a=8;

int funcion()
{
    int b = 23;
    return a + b + x; //mips devuelve a través de r2
}
```



Observe:
* Cómo la dirección de las variables globales se guarda en $28
* Cómo el contenido de la variable local a es recuperada en las líneas 3 y 4
* Cómo se hace la primera suma a+23
* Cómo se obtiene la variable global mediante el desplazamiento de x
* Cómo las variables globales siempre son indirecciones
* Y cómo se usa el salto retardado

mips-linux-gnu-gcc -O1 -c --save-temps func.c
cat func.s

        lui     $28,%hi(__gnu_local_gp)
        addiu   $28,$28,%lo(__gnu_local_gp)
        lui     $2,%hi(a)
        lw      $2,%lo(a)($2)
        addiu   $2,$2,23
        lw      $3,%got(x)($28)
        lw      $3,0($3)
        jr      $31
        addu    $2,$2,$3

El archivo intermedio usa directivas como got(), que utiliza para generar el binario, mientras  que el desensamblado muestra el resultado crudo:

mips-linux-gnu-objdump --syms -D func.o

   0:   3c1c0000        lui     gp,0x0
   4:   279c0000        addiu   gp,gp,0
   8:   3c020000        lui     v0,0x0
   c:   8c420000        lw      v0,0(v0)
  10:   24420017        addiu   v0,v0,23
  14:   8f830000        lw      v1,0(gp)
  18:   8c630000        lw      v1,0(v1)
  1c:   03e00008        jr      ra
  20:   00431021        addu    v0,v0,v1

