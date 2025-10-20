# 0 "loop0.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 0 "<command-line>" 2
# 1 "loop0.c"
double c[1024];
void function(){
for(register int i=0;i<1024;i++)
        c[i]=c[i]/2.0;
}
