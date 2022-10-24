# Primera parte: Ejemplo de compilación de códigos con saltos y loads retardados

El concepto _delayed load_ o carga retardada se refiere al ciclo perdido detrás de una instrucción _load_ cuando tiene una dependencia verdadera.

A pesar de la similitud del nombre con _delayed bracnch_, no consiste en una redefinición del concepto de _load_. Sin embargo, en códigos no optimizados, ambos coinciden en la necesidad de insertar una nop tras la instrucción para evitar un mal funcionamiento del código. Veamos un ejemplo:

Considere el siguiente código C (loadbranch.c):
```c
int a;

int funcion()
{
register int var1;
if(a<5)
        var1=2;
else
        var1=3;
return a+var1;
}
```
Introducimos por primera vez la palabra clave _register_ delante de una declaración de un entero. Es una forma de pedirle al compilador que use un registro para guardar la variable, aunque en compiladores y códigos modernos ya resulta innecesario.

Como la arquitectura de los procesadores MIPS R2000 es parecida a la de DLX, compilamos con la opción -mips1 (R2000) y forzosamente, -mfp32 (registros flotantes de 32 bits):

```sh
mips-linux-gnu-gcc -save-temps  -c loadbranch.c -mips1 -mfp32
```

Del código ensamblador (loadbranch.s) identificaremos cinco aspectos claves:

## 1 Almacenamiento (backup) en pila y restauración. 

Cualquier función tiene al principio el backup en pila de los valores en registros que va a usar la función, y restauración al final, para dejar todo como estaba:

        addiu   $sp,$sp,-8
        sw      $fp,4($sp)   # push $fp
        sw      $16,0($sp)   # push $16
...
        lw      $fp,4($sp)   # pop $fp
        lw      $16,0($sp)   # pop $16
        addiu   $sp,$sp,8
        jr      $31
        nop

Precisamente, uno de los registros preservados, el registro $fp (marco de pila), dentro de una función, indica la zona de la pila reservada para todo lo que se haga dentro de la función. Como se observa, la pila en mips crece en direcciones decrecientes. 

No todos los registros que use una función deben ser preservados. En algunos casos, se da su valor por perdido cuando se llama una función, como $2.

## 2 Implementación de un bloque if-then-else

La parte correspondiente al _if-then-else_ (previamente, la dirección de la variable global a se almacenó en $2, procedente de la zona de memoria global, apuntada siempre por el registro $28 y con desplazamiento %got(a) que en realidad es el valor cero, porque sólo hay una variable global) está asociado a una pareja de instrucciones de salto condicional (beq) y salto incondicional (b):

```asm
        lw      $2,%got(a)($28)
        nop
        lw      $2,0($2)
        nop
        slt     $2,$2,5
        beq     $2,$0,$L2
        nop
        li      $16,2
        b       $L3
        nop
$L2:
        li      $16,3
$L3:
```

## 3 Saltos y branch retardados

Observe las instrucciones nop tras cada uno de los branch, y detrás del load, debido a la dependencia verdadera de $2.

        lw      $2,0($2)
        nop

        beq     $2,$0,$L2
        nop

        b       $L3
        nop

## 4 Suma de a+var1

Como la variable $2 almacenaba la dirección de a, y el registro $16 es la variable var1, la suma se ejecuta así:

        lw      $2,%got(a)($28)  # Obtenemos la dirección de a desde la dirección de la zona global
        nop
        lw      $2,0($2) # Nos volvemos a descargar a porque el código no está optimizado
        nop
        addu    $2,$16,$2


## 5 Valor de retorno de la función

En mips, las funciones de tipo entero utilizan el registro $2 para devolver el valor entero de la función, por lo que no es necesario hacer nada más.



# Segunda parte: Ejemplo de compilación de códigos con lazos desenrollados


Considere el siguiente código:


```c
double c[1024];
void function(){
for(register int i=0;i<1024;i++)
        c[i]=c[i]/2.0;
}

```

Compile el código e identifique la estructura del lazo en el código ensamblador:

```bsh
mips-linux-gnu-gcc -save-temps -c func.c
``` 
```asm
        move    $16,$0  # Inicialización de $i
        b       $L2
        nop

$L3:
        lw      $3,%got(c)($28)
        sll     $2,$16,3
        addu    $2,$3,$2
        ldc1    $f2,0($2)
        lui     $2,%hi($LC0)
        ldc1    $f0,%lo($LC0)($2)
        div.d   $f0,$f2,$f0
        lw      $3,%got(c)($28)
        sll     $2,$16,3
        addu    $2,$3,$2
        sdc1    $f0,0($2)
        addiu   $16,$16,1
$L2:
        slt     $2,$16,1024
        bne     $2,$0,$L3
        nop

        # ... 

        jr      $31
```

Observe a continuación un código optimizado con desenrollamiento de lazos:

```bsh
mips-linux-gnu-gcc -O5 -save-temps -funroll-all-loops --param max-unroll-times=4   -c func.c
``` 


```assembly
        addiu   $5,$5,32
        ldc1    $f14,-32($5)
        ldc1    $f16,-24($5)
        ldc1    $f18,-16($5)
        ldc1    $f2,-8($5)
        mul.d   $f8,$f14,$f0
        mul.d   $f6,$f16,$f0
        mul.d   $f4,$f18,$f0
        sdc1    $f8,-32($5)
        mul.d   $f10,$f2,$f0
        sdc1    $f6,-24($5)
        sdc1    $f4,-16($5)
        bne     $5,$3,$L2
        sdc1    $f10,-8($5)

```

Estudie el resultado con un número de iteraciones no divisible por cuatro (1023)