

```c
double c[1024];
void function(){
for(register int i=0;i<1023;i++)
        c[i]=c[i]/2.0;
}

```

```bsh
mips-linux-gnu-gcc -save-temps -c func.c
``` 
```asm
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
        slt     $2,$16,1023
        bne     $2,$0,$L3
        nop

        nop
        move    $sp,$fp
        lw      $fp,4($sp)
        lw      $16,0($sp)
        addiu   $sp,$sp,8
        jr      $31
``` 

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