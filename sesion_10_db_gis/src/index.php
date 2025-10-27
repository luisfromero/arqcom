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
        .data-table { width: 100%; border-collapse: collapse; margin-top: 1.5em; }
        .data-table th, .data-table td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        .data-table thead { background-color: #f2f2f2; }
        .data-table tr:nth-child(even) { background-color: #f9f9f9; }
        .data-table tr:hover { background-color: #f1f1f1; }
        h2 { border-bottom: 2px solid #eee; padding-bottom: 10px; margin-top: 2em; }
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
        $rootpass = 'mi_root_password';
        $db = 'mi_database';
        $db_arqcom = 'arqcom';

        /**
         * Inserta 100 filas con datos aleatorios en la tabla 'posicion'.
         * @param mysqli $conn El objeto de conexión a la base de datos.
         * @return string Mensaje HTML con el resultado de la operación.
         */
        function insertarPosicionesAleatorias($conn)
        {
            // Preparamos la consulta una sola vez para mayor eficiencia.
            $stmt = $conn->prepare("INSERT INTO ubicacion (alumno, tiempo, latitud, longitud) VALUES (?, NOW(), ?, ?)");
            if ($stmt === false) {
                return '<div class="status error">Error al preparar la consulta: ' . htmlspecialchars($conn->error) . '</div>';
            }

            // Vinculamos los parámetros a las variables. 'idd' significa (integer, double, double).
            $stmt->bind_param("idd", $alumno_id, $latitud, $longitud);

            $errores = 0;
            for ($i = 0; $i < 100; $i++) {
                // Generamos los datos aleatorios para cada fila
                $alumno_id = rand(1, 25); // ID de alumno aleatorio entre 1 y 25
                $latitud = (rand() / getrandmax()) * 180 - 90;   // Latitud aleatoria entre -90 y 90
                $longitud = (rand() / getrandmax()) * 360 - 180; // Longitud aleatoria entre -180 y 180

                // Ejecutamos la consulta con los nuevos datos
                if (!$stmt->execute()) {
                    $errores++;
                }
            }

            $stmt->close();

            if ($errores === 0) {
                return '<div class="status success">¡Se insertaron 100 filas aleatorias en la tabla <strong>ubicacion</strong> correctamente!</div>';
            } else {
                return '<div class="status error">Ocurrieron ' . $errores . ' errores durante la inserción de filas.</div>';
            }
        }

        /**
         * Muestra las últimas 50 filas de la tabla 'ubicacion' en una tabla HTML.
         * @param mysqli $conn El objeto de conexión a la base de datos.
         * @return string La tabla HTML con los datos o un mensaje de error/informativo.
         */
        function mostrarPosiciones($conn)
        {
            $sql = "SELECT id, alumno, tiempo, latitud, longitud FROM ubicacion ORDER BY tiempo DESC LIMIT 50";
            $result = $conn->query($sql);

            if ($result === false) {
                return '<div class="status error">Error al consultar la tabla: ' . htmlspecialchars($conn->error) . '</div>';
            }

            if ($result->num_rows > 0) {
                $html = '<h2>Últimas 50 Posiciones Registradas</h2>';
                $html .= '<table class="data-table">';
                $html .= '<thead><tr><th>ID</th><th>Alumno</th><th>Tiempo</th><th>Latitud</th><th>Longitud</th></tr></thead>';
                $html .= '<tbody>';
                while ($row = $result->fetch_assoc()) {
                    $html .= '<tr>';
                    $html .= '<td>' . htmlspecialchars($row['id']) . '</td>';
                    $html .= '<td>' . htmlspecialchars($row['alumno']) . '</td>';
                    $html .= '<td>' . htmlspecialchars($row['tiempo']) . '</td>';
                    $html .= '<td>' . number_format($row['latitud'], 4) . '</td>';
                    $html .= '<td>' . number_format($row['longitud'], 4) . '</td>';
                    $html .= '</tr>';
                }
                $html .= '</tbody></table>';
                return $html;
            } else {
                return '<div class="status">No hay datos en la tabla <strong>ubicacion</strong> para mostrar.</div>';
            }
        }


        try {
            // Usamos la extensión mysqli que instalamos en el Dockerfile
            $conn = new mysqli($host, 'root', $rootpass, $db_arqcom);
            if ($conn->connect_error) {
                throw new Exception("Error de conexión: " . htmlspecialchars($conn->connect_error));
            }
            echo '<div class="status success">¡Conexión a la base de datos <strong>' . $db_arqcom . '</strong> exitosa!</div>';
            
            // Llamamos a la función para insertar los datos
            //echo insertarPosicionesAleatorias($conn);

            // Llamamos a la función para mostrar los datos
            echo mostrarPosiciones($conn);

            $conn->close();
        } catch (Exception $e) {
            echo '<div class="status error">' . $e->getMessage() . '</div>';
        }
        ?>
    </div>
</body>
</html>