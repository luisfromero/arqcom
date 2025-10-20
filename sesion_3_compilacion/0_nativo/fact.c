// fact.c
// Nota: La sintaxis 'extern "C"' es para C++, en C puro no es necesaria,
// pero no hace daño y es una buena práctica si se planea usar desde C++.
#ifdef __cplusplus
extern "C" {
#endif

int fact(int x)
{
    // Esto no es el factorial, pero usamos la función del ejemplo.
    return x * (x - 1);
}

#ifdef __cplusplus
}
#endif