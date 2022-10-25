extern int x;     
// La dirección del área global se guarda en r28
// El símbolo es el desplazamiento
int a=8;

int funcion()
{
    int b = 23;
    return a + b + x; //mips devuelve a través de r2
}