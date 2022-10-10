Compilar:
```mips-linux-gnu-gcc -c --save-temps datos.c```

cat datos.s
cat datos.i

Desensamblador:

```mips-linux-gnu-objdump --syms   -D datos.o```

(la opción --syms muestra la tabla de símbolos)