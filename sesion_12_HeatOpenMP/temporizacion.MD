# Temporización de aplicaciones en C++

Desde hace pocos años, la temporización de aplicaciones en Windows y Linux utiliza la librería estándar chrono. Su precisión es muy alta (nanosegundos), y su uso, muy sencillo:

Es necesario incluir esta línea:

```C
#include <chrono>
```

A continuación, se captura la hora del sistema, con precisión altísima, dos veces: antes y después del cálculo que quieres medir:

```C
std::chrono::time_point<std::chrono::high_resolution_clock> t1 = std::chrono::high_resolution_clock::now();

// Alguna operación de pocos flops (por ejemplo, heat(), que tiene 500x500x4 flops)

std::chrono::time_point<std::chrono::high_resolution_clock> t2 = std::chrono::high_resolution_clock::now();
```

Finalmente, se calcula el tiempo transcurrido:

```C
double t = (double)(t2 - t1).count() / 1000000000;
```

En caso de que la función a temporizar esté dentro de un bucle junto a otras operaciones, se acumulan los tiempos (variables t1 y t2):

```C
double tiempoTotal=0;
for{
    t1 = std::chrono::high_resolution_clock::now();
    // Alguna operación de pocos flops (por ejemplo, heat(), que tiene 500x500x4 flops)
    t2 = std::chrono::high_resolution_clock::now();
    double t = (double)(t2 - t1).count() / 1000000000;
    tiempototal+=t;
    // Aquí habría operaciones (dentro del buble) que no quieres considerar en la temporización
    copyimage(); // o similar
}
cout << tiempoTotal;
```

