# Práctica 10: Servidores Web, Bases de Datos y GIS con Docker

En esta práctica, daremos un paso más allá de un único contenedor para orquestar un entorno de desarrollo web completo utilizando Docker Compose. Implementaremos una arquitectura de tres capas que es fundamental en el desarrollo de aplicaciones web modernas.

## Objetivos de la Práctica

1.  **Orquestación de Servicios**: Aprender a definir y gestionar una aplicación multi-contenedor con `docker-compose.yml`.
2.  **Arquitectura Web**: Implementar una pila de servicios común:
    *   Un **servidor web** (Nginx) con un procesador de lenguaje (PHP).
    *   Un **servidor de base de datos** (MySQL).
    *   Una **herramienta de administración** de bases de datos (phpMyAdmin).
3.  **Desarrollo Backend**: Crear un script PHP que se conecte a la base de datos para almacenar y recuperar información.
4.  **Conceptos de Redes**: Comprender la exposición de puertos en Docker y, como tarea avanzada, investigar el reenvío de puertos (port forwarding) en un router doméstico para hacer accesible un servicio desde Internet.

## Prerrequisitos

-   **Docker Desktop**: Asegúrate de tenerlo instalado y en ejecución en tu sistema.
-   **Editor de Código**: Se recomienda un editor como VS Code.
-   **Línea de Comandos**: Familiaridad básica con la terminal de tu sistema operativo.

---

## Puesta en Marcha del Entorno

En este directorio encontrarás un archivo `docker-compose.yml` que define los tres servicios necesarios. Antes de iniciar, revisa su contenido para entender cómo se relacionan:

-   **`db`**: El servicio de base de datos MySQL. Fíjate en las variables de entorno (`environment`) que establecen la contraseña de root y el nombre de la base de datos inicial.
-   **`phpmyadmin`**: La interfaz gráfica para administrar la base de datos. Se conecta al servicio `db`.
-   **`web`**: El servidor web Nginx, que está configurado para trabajar con PHP. Monta el código de la carpeta `src` en su directorio web.

### Pasos para Iniciar

1.  Abre una terminal en este directorio (`sesion_10_db_gis`).
2.  Ejecuta el siguiente comando para construir y levantar todos los servicios en segundo plano:

    ```bash
    docker-compose up -d --build
    ```

3.  Una vez que los contenedores estén en marcha, verifica que puedes acceder a los servicios desde tu navegador:
    *   **Servidor Web**: http://localhost:8080
    *   **phpMyAdmin**: http://localhost:8081

    Para iniciar sesión en phpMyAdmin, utiliza el usuario `root` y la contraseña que has configurado en el archivo `docker-compose.yml` para el servicio `db`.

---

## Ejercicio: Creación de un Log de Ubicaciones

Tu tarea es desarrollar un script PHP que registre datos de ubicación en la base de datos.

### 1. Crear la Tabla en la Base de Datos

Usa **phpMyAdmin** para crear una tabla llamada `locations` en la base de datos `mi_database` (o la que hayas definido en `docker-compose.yml`). La tabla debe tener, como mínimo, los siguientes campos:

-   `id`: `INT`, `AUTO_INCREMENT`, `PRIMARY KEY`
-   `latitude`: `DECIMAL(10, 8)`
-   `longitude`: `DECIMAL(11, 8)`
-   `timestamp`: `DATETIME`, con valor por defecto `CURRENT_TIMESTAMP`
-   `description`: `VARCHAR(255)`, opcional.

### 2. Desarrollar el Script `log.php`

Crea un archivo llamado `log.php` dentro de la carpeta `src/`. Este script debe:

1.  **Recibir datos por URL**: Debe ser capaz de leer parámetros `lat`, `lon` y `desc` de la URL (usando `$_GET`).
    *   Ejemplo de llamada: `http://localhost:8080/log.php?lat=36.7153&lon=-4.4776&desc=Universidad_de_Malaga`
2.  **Conectarse a la base de datos**: Utiliza las credenciales del `docker-compose.yml` para establecer una conexión con el servicio `db`.
3.  **Insertar los datos**: Si los parámetros `lat` y `lon` están presentes, ejecuta una consulta `INSERT` para guardar la nueva ubicación en la tabla `locations`.
4.  **Mostrar un resultado**: Imprime un mensaje de confirmación (ej: "Ubicación registrada correctamente.") o un mensaje de error si algo falla.

### 3. (Opcional) Mostrar los Registros

Modifica el archivo `index.php` para que, además de comprobar la conexión, realice una consulta `SELECT * FROM locations` y muestre los registros almacenados en una tabla HTML.

## Tarea Extra: ¡Publica tu Servicio en Internet!

Investiga cómo configurar el **reenvío de puertos (port forwarding)** en el router de tu casa. El objetivo es que tu servidor web sea accesible desde fuera de tu red local.

1.  **Averigua la IP privada** de tu ordenador en tu red local.
2.  **Accede a la configuración de tu router** (normalmente a través de una dirección como `192.168.1.1`).
3.  **Busca la sección "Port Forwarding"**, "NAT" o "Servidores Virtuales".
4.  **Crea una nueva regla** para redirigir el tráfico del puerto `8080` (o el que prefieras) de tu IP pública al puerto `8080` de la IP privada de tu ordenador.

Si lo consigues, cualquier persona en Internet podrá acceder a tu aplicación usando tu IP pública seguida del puerto (ej: `http://<TU_IP_PUBLICA>:8080`).

> **Nota de seguridad**: Exponer servicios a Internet conlleva riesgos. Hazlo solo para esta práctica y considera cerrar el puerto cuando termines. Nunca expongas puertos de bases de datos o de administración (como el de phpMyAdmin) a menos que sea estrictamente necesario y esté debidamente securizado.

## Entrega de la Práctica

Para entregar el ejercicio, prepara un correo electrónico con el siguiente contenido:

-   **Asunto**: `[AC] Práctica 10 - Tu Nombre y Apellidos`
-   **Cuerpo del correo**:
    -   Si has completado la tarea extra, incluye el enlace público a tu servidor web (ej: `http://80.58.XX.XX:8080/index.php`).
    -   Breve explicación de cualquier dificultad encontrada o decisión de diseño tomada.
-   **Archivos adjuntos**: Comprime en un `.zip` la carpeta `sesion_10_db_gis` completa, incluyendo todos los archivos (`docker-compose.yml`, `nginx.conf`, y el contenido de la carpeta `src/`).
