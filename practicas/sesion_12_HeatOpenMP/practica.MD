## Entrega 1:

* 2 puntos

Se pide un código simple en c o c++ en el que se intente la ejecución en varios hilos con OpenMP de cualquier tarea, aunque sea la simple impresión de un "Hola mundo". Asimismo, se pide que se indique cómo se ha compilado, y un pantallazo de la ejecución. Se valora la originalidad. La realización de la tarea para la máxima calificación  no debería llevar más de media hora.

## Entrega 2:

* 8 puntos

En el código fuente principal heatOpenMp.cpp dentro del archivo [heatClientServerOpenMp2023.zip](heatClientServerOpenMp2023.zip) se ha paralelizado (en el servidor) la parte más costosa del código (la que consume más tiempo de CPU). Sin embargo, el código no es muy eficiente. 

Si se ponen barreras, la simulación es lenta debido a que hay un thread que, además de hacer cálculos, se encarga de hacer 
la entrada salida. Y si no se ponen, entonces hay una región más
lenta que otra. 

Se pide, en este ejercicio, aislar el envío de datos de los cálculos (se enviaría asíncronamente el estado de la placa).

Puede hacerse con un thread posix (thread_emisor), aunque otra
posibilidad sería utilizar openmp anidado:

* En el primer nivel, tres threads (uno recibe, otro envía y el tercero calcula)
* El thread que calcula se paralelizaría en un segundo nivel, creando tantos threads como se quiera