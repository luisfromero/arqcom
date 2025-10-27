<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Docker: Nginx+PHP y MySQL</title>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; background-color: #f0f2f5; color: #333; margin: 2em; }
        .container { max-width: 700px; margin: auto; background: white; padding: 2em; border-radius: 10px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); }
        h1 { color: #0d6efd; }
        .status { padding: 12px; border-radius: 6px; margin-top: 1em; font-weight: 500; }
        .success { background-color: #d1e7dd; border: 1px solid #a3cfbb; color: #0a3622; }
        .error { background-color: #f8d7da; border: 1px solid #f1aeb5; color: #58151c; }
    </style>
</head>
<body>
    <div class="container">
        <h1>¡Entorno de dos contenedores funcionando!</h1>
        <p>Si ves esto, el contenedor <strong>web</strong> (Nginx + PHP) está sirviendo esta página correctamente.</p>

        <h2>Estado de la conexión al contenedor <code>db</code></h2>
        <?php
        // 'db' es el nombre del servicio de MySQL en docker-compose.yml
        $host = 'db'; 
        $user = 'mi_user';
        $pass = 'mi_password';
        $db = 'mi_database';

        try {
            // Usamos la extensión mysqli que instalamos en el Dockerfile
            $conn = new mysqli($host, $user, $pass, $db);
            if ($conn->connect_error) {
                throw new Exception("Error de conexión: " . $conn->connect_error);
            }
            echo '<div class="status success">¡Conexión a la base de datos <strong>' . $db . '</strong> exitosa!</div>';
            $conn->close();
        } catch (Exception $e) {
            echo '<div class="status error">Error al conectar a la base de datos: ' . $e->getMessage() . '</div>';
        }
        ?>
    </div>
</body>
</html>