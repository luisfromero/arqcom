# Arquitectura del Servidor de Simulación de Calor

Este documento describe la arquitectura del proyecto del servidor de simulación de calor, que ha sido refactorizado desde un único archivo monolítico (`heatServer.c`) a una estructura modular y reutilizable.

## Objetivos del Diseño

1.  **Modularidad**: Separar la lógica de la simulación de la lógica del servidor de red.
2.  **Reutilización**: Crear una biblioteca (`.so`) con el núcleo de la simulación para que pueda ser utilizada por diferentes aplicaciones (el servidor en C, una futura API en Python, etc.).
3.  **Mantenibilidad**: Facilitar la comprensión, modificación y depuración del código.

## Estructura del Proyecto

El proyecto se divide en tres componentes principales:

### 1. `libheat` (Biblioteca de Simulación)

Este es el corazón del proyecto. Contiene toda la lógica para inicializar la placa de calor, aplicar puntos de temperatura y ejecutar los pasos de la simulación.

*   **`heat.h`**: El archivo de cabecera que define la **interfaz pública** de la biblioteca. Declara las funciones que pueden ser llamadas desde otros programas.
*   **`heat.c`**: La **implementación** de las funciones de simulación.

El resultado de su compilación es `libheat.so`, una biblioteca compartida (DLL en Windows).

### 2. `server` (Ejecutable del Servidor)

Este componente se encarga exclusivamente de la comunicación por red. Su responsabilidad es aceptar conexiones de clientes, recibir comandos y enviar los resultados de la simulación.

*   **`server.c`**: Contiene la función `main`. Orquesta la creación del socket, el hilo de escucha y el bucle principal que llama a la biblioteca `libheat`.
*   **`socket_utils.h` / `socket_utils.c`**: Módulo de utilidad que encapsula la lógica de bajo nivel para crear y gestionar sockets.

El resultado de su compilación es el ejecutable `heat_server`.

### 3. `Makefile` (Sistema de Construcción)

Este archivo automatiza todo el proceso de compilación. Define cómo compilar la biblioteca y el servidor, y cómo enlazarlos correctamente.

*   **`make` o `make all`**: Compila `libheat.so` y `heat_server`.
*   **`make clean`**: Elimina todos los archivos generados durante la compilación (`.o`, `.so`, ejecutables).

## Flujo de Trabajo

1.  **Compilación**: Al ejecutar `make`, el `Makefile` primero compila `heat.c` para crear `libheat.so`. Luego, compila `server.c` y `socket_utils.c` y los enlaza con la biblioteca `libheat.so` para crear el ejecutable final `heat_server`.

2.  **Ejecución**: Al lanzar `./heat_server <puerto>`, el programa:
    *   Abre un socket y espera a un cliente.
    *   Una vez conectado, crea un hilo para escuchar los comandos del cliente (ej: aplicar calor).
    *   El hilo principal entra en un bucle donde, repetidamente:
        1.  Llama a `compute_heat_step()` de `libheat.so` para avanzar la simulación.
        2.  Llama a `copy_image_to_buffer()` de `libheat.so` para obtener el estado actual.
        3.  Envía el estado al cliente a través del socket.