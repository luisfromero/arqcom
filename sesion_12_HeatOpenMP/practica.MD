## Entrega 1:

* 2 puntos

Se pide un código simple en c o c++ en el que se intente la ejecución en varios hilos con OpenMP de cualquier tarea, aunque sea la simple impresión de un "Hola mundo". Asimismo, se pide que se indique cómo se ha compilado, y un pantallazo de la ejecución. Se valora la originalidad. La realización de la tarea para la máxima calificación  no debería llevar más de media hora.

## Entrega 2:

* 8 puntos

En el código fuente principal heatOpenMp.cpp dentro del archivo [heatClientServerOpenMp2023.zip](heatClientServerOpenMp2023.zip) se ha paralelizado (en el servidor) la parte más costosa del código (la que consume más tiempo de CPU). Sin embargo, el código contiene errores, además de unas barreras que hacen que pierda más tiempo del necesario. Se pide, en primer lugar, implementar una medida de rendimiento (ciclos/segundo, mflops, etc...), para a continuación eliminar las barreras y sustituirlas por otros mecanismos que permitan que el código se ejecute de forma correcta, o al menos, visualmente correcta, pero de forma más eficiente.

**_Nota:_** *Las fronteras entre los dominios de la solución de cada thread son la parte más crítica de la resolución paralela. Sin las barreras, uno de los threads puede ir varios pasos de tiempo por delante de los demás, lo que provoca que afecta a los valores de temperatura en las fronteras y que la ejecución resulte incorrecta. La alternativa a la barrera es que los threads sólo se sincronicen con sus vecinos.