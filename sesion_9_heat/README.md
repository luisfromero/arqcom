# Cliente servidor para resolver y visualizar la difusión de calor en una placa

El objetivo de esta práctica es poner en marcha un servidor que resuelve un sencillo problema de difusión o advección de calor mediante diferencias finitas (con un método explícito de Euler).

El servidor recibe, mediante un socket, la conexión de un cliente que puede visualizar el estado de la placa, pero que además, puede modificar la temperatura en algunos puntos de la misma.

Con el objeto de acelerar la visualización, se utilizan temperaturas entre 0 y 255 unidades, que se transfieren redondeadas a enteros de un byte. Se discretiza la placa con una malla de 500x500 celdas, por lo que el cliente recibe periódicamente el estado de la placa mediante bloques de 250000 bytes. 

Se muestra a continuación un pantallazo del cliente:

<br/><p style='text-align:center;'>
<img src=cliente.png width=200>
</p><br/>

El esquema de 5 puntos calcula la temperatura mediante el simple algoritmo:

```c
for (i = 1; i < SIZEY-1; i++)
	for (j = 1; j < SIZEX - 1; j++)
		w[i][j] = (u[i-1][j] + u[i+1][j] + u[i][j- 1] + u[i][j + 1]  )* 0.25;
```

