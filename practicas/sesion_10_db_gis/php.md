# Guía Rápida de PHP para este Entorno Docker

Este documento es una introducción rápida a PHP, explicando cómo funciona dentro de nuestro entorno Docker y cómo puedes interactuar con él.

## 1. ¿Qué es PHP?

PHP (acrónimo recursivo de *PHP: Hypertext Preprocessor*) es un lenguaje de programación de "scripting" del lado del servidor. Esto significa que, a diferencia de HTML, CSS o JavaScript, el código PHP **se ejecuta en el servidor** (en nuestro caso, dentro del contenedor `web`) antes de que la página se envíe al navegador del usuario.

Su principal propósito es crear páginas web dinámicas:

-   Puede conectarse a bases de datos (como MySQL en nuestro contenedor `db`).
-   Puede procesar datos de formularios.
-   Puede generar contenido HTML personalizado basado en lógica, bucles o condiciones.

El navegador del usuario **nunca ve el código PHP**. Solo recibe el resultado final, que es puro HTML.

## 2. ¿Cómo Funciona en Nuestro Entorno? (El Ciclo de Vida de una Petición)

Cuando accedes a `http://localhost:8080/index.php`, ocurre lo siguiente:

1.  **Navegador -> Nginx**: Tu navegador envía una petición al puerto `8080` de tu máquina, que Docker redirige al puerto `80` del contenedor `web`. El servidor **Nginx** recibe la petición.

2.  **Nginx -> PHP-FPM**: Nginx ve que el archivo solicitado es `.php`. Según su configuración (`nginx.conf`), en lugar de servir el archivo directamente, se lo pasa al procesador de PHP (**PHP-FPM**) a través del puerto `9000` dentro del contenedor.

3.  **PHP-FPM Ejecuta el Código**: PHP-FPM abre el archivo `index.php` y ejecuta todo lo que está dentro de las etiquetas `<?php ... ?>`.
    -   En nuestro `index.php`, se conecta a la base de datos `db`.
    -   Verifica si la conexión fue exitosa.
    -   Genera un bloque de HTML (`<div class="status success">...</div>`) con el resultado.

4.  **PHP-FPM -> Nginx**: PHP-FPM devuelve el resultado de su ejecución a Nginx. Este resultado ya no contiene código PHP, solo HTML, CSS y JavaScript.

5.  **Nginx -> Navegador**: Nginx envía esta respuesta final (el HTML puro) a tu navegador.

6.  **El Navegador Renderiza**: Tu navegador recibe el HTML y lo muestra como una página web normal.

### Sintaxis Básica

El código PHP se escribe dentro de etiquetas especiales.

```php
<!DOCTYPE html>
<html>
<head>
    <title>Ejemplo de PHP</title>
</head>
<body>

    <h1>Página de Ejemplo</h1>

    <p>Esto es HTML normal.</p>

    <?php
        // Esto es un comentario en PHP.
        // El código aquí se ejecuta en el servidor.

        $nombre = "Mundo"; // Las variables empiezan con '$'.
        $hora = date('H:i');

        // 'echo' se usa para imprimir texto en el HTML de salida.
        echo "<p>¡Hola, " . $nombre . "!</p>"; // El '.' se usa para concatenar strings.
        echo "<p>La hora actual del servidor es: " . $hora . "</p>";
    ?>

    <p>Esto también es HTML normal.</p>

</body>
</html>
```

## 3. PHP desde la Línea de Comandos (CLI)

PHP no solo sirve para la web. También es un potente lenguaje de scripting que puedes ejecutar directamente desde la terminal, similar a Python o Bash. Esto es útil para tareas de automatización, mantenimiento o scripts de consola.

### ¿Cómo usarlo en nuestro entorno Docker?

1.  **Accede al contenedor `web`**:
    Abre una terminal en la raíz de tu proyecto y ejecuta:
    ```bash
    docker-compose exec web bash
    ```
    Esto te dará una sesión de terminal `bash` dentro del contenedor `web`.

2.  **Navega al directorio de tu código**:
    El código de tu carpeta `src/` está montado en `/var/www/html/` dentro del contenedor.
    ```bash
    cd /var/www/html/
    ```

3.  **Ejecuta un script PHP**:
    Ahora puedes ejecutar cualquier archivo `.php` usando el comando `php`.
    ```bash
    # Ejecuta el archivo index.php y muestra su salida en la consola
    php index.php
    ```
    Verás el código HTML completo de la página impreso directamente en tu terminal.

4.  **Modo Interactivo**:
    También puedes iniciar una consola interactiva de PHP para probar código rápidamente.
    ```bash
    php -a
    ```
    Una vez dentro, puedes escribir código PHP y ver el resultado al instante. Para salir, presiona `Ctrl + C`.

¡Espero que esta guía te sea de gran ayuda para empezar a trabajar con PHP en este proyecto!
