<?php
/**
 * Endpoint para recibir y almacenar datos de GPSLogger.
 * 
 * Este script espera una petición GET con los siguientes parámetros:
 * - lat: Latitud (número de punto flotante)
 * - lon: Longitud (número de punto flotante)
 * - alumno_id: Identificador numérico del alumno
 * 
 * Ejemplo de URL de configuración en GPSLogger:
 * http://localhost:8080/log.php?lat=%lat&lon=%lon&alumno_id=1
 */

// --- 1. Configuración de la Base de Datos ---
$host = 'db';
$user = 'root'; // Usamos el usuario root como en index.php
$pass = 'mi_root_password';
$db_name = 'arqcom';

// --- 2. Obtención y Validación de Datos ---
$lat = $_GET['lat'] ?? null;
$lon = $_GET['lon'] ?? null;
$alumno_id = $_GET['alumno_id'] ?? null;

if (is_null($lat) || is_null($lon) || is_null($alumno_id)) {
    http_response_code(400); // Bad Request
    die("Error: Faltan parámetros requeridos (lat, lon, alumno_id).");
}

if (!is_numeric($lat) || !is_numeric($lon) || !is_numeric($alumno_id)) {
    http_response_code(400); // Bad Request
    die("Error: Los parámetros deben ser numéricos.");
}

// --- 3. Conexión a la Base de Datos ---
$conn = new mysqli($host, $user, $pass, $db_name);

if ($conn->connect_error) {
    http_response_code(500); // Internal Server Error
    // En una aplicación real, este error se registraría en un archivo de log, no se mostraría al cliente.
    die("Error de conexión al servidor de base de datos.");
}

// --- 4. Inserción de Datos ---
$stmt = $conn->prepare("INSERT INTO ubicacion (alumno, tiempo, latitud, longitud) VALUES (?, NOW(), ?, ?)");
if ($stmt === false) {
    http_response_code(500);
    die("Error al preparar la consulta SQL.");
}

// Vinculamos los parámetros: 'idd' significa (integer, double, double)
$stmt->bind_param("idd", $alumno_id, $lat, $lon);

if ($stmt->execute()) {
    echo "ok"; // Respuesta esperada por GPSLogger para confirmar la recepción.
} else {
    http_response_code(500);
    die("Error al ejecutar la inserción en la base de datos.");
}

$stmt->close();
$conn->close();
