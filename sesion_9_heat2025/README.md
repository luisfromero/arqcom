# Entorno de Desarrollo con Docker para Arquitectura de Computadores

Este directorio contiene la configuración para levantar un entorno de desarrollo autocontenido y portable usando Docker. El objetivo es proporcionar un ambiente de trabajo consistente para todos, con todas las herramientas necesarias para las prácticas de la asignatura preinstaladas.

## Componentes del Entorno

1.  **`Dockerfile`**: Es la "receta" de nuestra máquina de desarrollo. Define el sistema operativo base (Ubuntu 22.04) y todas las herramientas que se instalan sobre él:
    *   Compiladores `gcc` y `g++`.
    *   Cadena de herramientas de compilación cruzada para `RISC-V`.
    *   Entorno de `Python 3` con `pip`, `fastapi` y `uvicorn`.
    *   Utilidades de consola como `nano`, `tree`, `man`, etc.
    *   Configura `bash` como el shell por defecto.

2.  **`docker-compose.yml`**: Es el "orquestador". Utiliza el `Dockerfile` para construir la imagen y define cómo se debe ejecutar nuestro contenedor:
    *   Nombra el proyecto como `arqcom` y el contenedor como `arqcom_container`.
    *   **Mapea las carpetas de prácticas**: Vincula las carpetas `sesion_*` de tu máquina local a la carpeta `/app` dentro del contenedor, para que puedas editar los archivos con tus herramientas locales y los cambios se reflejen instantáneamente.
    *   **Expone puertos**: Permite la comunicación entre el contenedor y tu máquina (por ejemplo, para acceder a servicios web).
    *   Asegura que el contenedor se mantenga en ejecución.

---

## Cómo Usar el Entorno con VS Code y Dev Containers

La forma más cómoda de trabajar es conectar Visual Studio Code directamente al contenedor en ejecución. Esto te permite usar la potencia de VS Code (editor, terminal, depurador) como si estuviera instalado dentro del contenedor.

### Prerrequisitos

1.  **Docker Desktop**: Asegúrate de que esté instalado y en ejecución.
2.  **Visual Studio Code**: El editor de código.
3.  **Extensión "Dev Containers"**: Instálala en VS Code. Búscala por `ms-vscode-remote.remote-containers` en el mercado de extensiones.

### Pasos para Conectar

1.  **Iniciar el Contenedor**:
    Abre una terminal en este directorio (`sesion_9_heat2025`) y ejecuta el siguiente comando. Esto construirá la imagen (la primera vez) y pondrá en marcha el contenedor en segundo plano.
    ```shell
    docker-compose up -d --build # Usar --build la primera vez o al modificar el Dockerfile
    ```

2.  **Conectar VS Code al Contenedor**:
    *   Abre Visual Studio Code.
    *   Haz clic en el **icono verde de Conexión Remota** en la esquina inferior izquierda de la ventana.
    !Icono Remoto
    *   Se desplegará un menú en la parte superior. Selecciona la opción **"Attach to Running Container..."**.
    *   En la lista que aparece, elige el contenedor llamado `/arqcom_container`.

3.  **¡Listo para Trabajar!**
    *   Se abrirá una nueva ventana de VS Code. Ahora estás "dentro" del contenedor.
    *   El explorador de archivos de la izquierda te mostrará el contenido del directorio `/app`, donde verás todas tus carpetas de prácticas.
    *   Puedes abrir una terminal integrada (`Ctrl + ñ` o `Terminal > New Terminal`), y estarás en un shell `bash` dentro del entorno Ubuntu, con todas las herramientas listas para usar.

### Uso Básico

*   **Compilar un programa**: Navega a la carpeta de una práctica y compila como lo harías normalmente.
    ```bash
    cd /app/sesion_3_compilacion
    gcc mi_programa.c -o mi_programa
    ```
*   **Ejecutar un servidor web**:
    ```bash
    cd /app/sesion_9_heat2025
    uvicorn main:app --host 0.0.0.0 --port 8010 --reload
    ```
*   **Ejecutar el Servidor de Simulación (C)**:
    El proyecto incluye un servidor de simulación de calor escrito en C con una arquitectura modular. Para más detalles sobre su diseño, consulta el archivo heatServer.md.
    
    Para compilarlo y ejecutarlo:
    ```bash
    # Navega al directorio del proyecto
    cd /app/sesion_9_heat2025
    # Compila todo el proyecto (biblioteca y ejecutable)
    make
    # Ejecuta el servidor en el puerto 8010.
    # Es necesario indicar al sistema que busque la biblioteca libheat.so en el directorio actual (.).
    LD_LIBRARY_PATH=. ./heat_server 8010
    ```

### Cómo Depurar Código C/C++

El entorno está configurado para facilitar la depuración de código C/C++ directamente desde VS Code.

1.  **Abre un archivo `.c`** que quieras depurar (por ejemplo, `mi_programa.c`).
2.  **Pon un punto de ruptura**: Haz clic en el margen izquierdo, junto al número de línea donde quieres que la ejecución se detenga.
3.  **Inicia la depuración**:
    *   Ve a la vista "Ejecutar y depurar" en la barra lateral de VS Code (icono de play con un bicho).
    *   Asegúrate de que en el menú desplegable de arriba esté seleccionada la configuración **"(GDB) Compilar y Depurar Archivo"**.
    *   Presiona la tecla **F5** o haz clic en el icono de play verde.

Automáticamente, VS Code ejecutará la tarea de compilación (creando un ejecutable con símbolos de depuración) y luego iniciará el depurador, deteniéndose en el punto de ruptura que estableciste.

---

### Finalizar la Sesión

1.  Para desconectarte, simplemente cierra la ventana de VS Code.
2.  Cuando hayas terminado de trabajar, puedes detener el contenedor abriendo una terminal en tu máquina (fuera de VS Code) y ejecutando:
    ```shell
    docker-compose down
    ```