# Práctica IoT: Entrega 2 - Automatización Inteligente con n8n

En esta entrega, construirás un proyecto en **n8n** que integrará disparadores temporales, APIs externas, una base de datos, y el protocolo MQTT. Demostrarás tu capacidad para crear flujos de trabajo que reaccionan a eventos y procesan datos de forma autónoma.

## 🎯 Objetivos

1.  **Dominar n8n**: Consolidar tu manejo de n8n para crear flujos de trabajo complejos con múltiples ramas y servicios.
2.  **Integración de APIs**: Extraer datos periódicamente de una fuente de datos externa (API REST).
3.  **Persistencia de Datos**: Almacenar la información recolectada en una base de datos MySQL.
4.  **Comunicación IoT**: Publicar y suscribirse a temas MQTT para comunicar eventos.
5.  **Gestión de Credenciales**: Configurar y utilizar de forma segura las credenciales para los distintos servicios.

---

## 🛠️ Tu Entorno de Trabajo

### 1. Servidor n8n Local
Deberás levantar tu propia instancia de n8n utilizando el `docker-compose.yml` proporcionado en la `sesion_11_iot`.
```bash
# En el directorio sesion_11_iot/
docker compose up -d
```
Accede a tu entorno de trabajo en `http://localhost:5678`.

### 2. Servicios Centralizados (proporcionados por el profesor)
Utilizaremos los siguientes servicios centralizados alojados en `casium.uma.es`:

*   **Broker MQTT**:
    *   **Host**: `casium.uma.es`
    *   **Puerto**: `8883`
    *   **Protocolo**: `mqtts` (MQTT sobre SSL/TLS)
    *   **Usuario**: `practicas`
    *   **Password**: `labdac`
*   **Base de Datos MySQL**:
    *   **Host**: `casium.uma.es`
    *   **Puerto**: `8090`
    *   **Base de Datos**: `mi_database`
    *   **Usuario/Password**: `mi_user` / `mi_password`
*   **Administrador de Base de Datos**:
    *   **URL**: `http://casium.uma.es/phpmyad/`
    *   Puedes usar esta herramienta para verificar que tus datos se están guardando correctamente.

### 3. Gestión de Credenciales
Antes de empezar, configura todas las credenciales necesarias en tu n8n. Usa como guía el archivo `credenciales_n8n.md` de la práctica 11. Necesitarás, como mínimo:
*   **Credencial para la API de AEMET** (o la que elijas).
*   **Credencial MQTT** para `casium.uma.es`.
*   **Credencial MySQL** para `casium.uma.es`.
*   **Credencial de Email** para poder enviar correos desde tu cuenta de la UMA (`@uma.es`).

---

## 📜 Requisitos del Flujo de Trabajo (Workflow)

Tu proyecto en n8n debe contener un único flujo de trabajo con **dos ramas principales**, cada una iniciada por un disparador (trigger) diferente.

### Rama 1: Recolección Periódica de Datos Meteorológicos

Esta rama se encargará de consultar el tiempo y distribuirlo.

1.  **Disparador (Trigger)**: Utiliza un nodo **Schedule** (o Cron) para que el flujo se ejecute automáticamente a intervalos regulares (ej: cada hora).
2.  **Fuente de Datos**: Con un nodo **HTTP Request**, consulta los datos de predicción de una API pública. Se recomienda la API de AEMET.
3.  **Acción 1: Guardar en Base de Datos**:
    *   Conéctate a la base de datos `mi_database`.
    *   Crea una tabla con un nombre que incluya tu primer nombre para evitar colisiones (ej: `felipe_prediccion_malaga`).
    *   Inserta los datos relevantes que has obtenido de la API en tu tabla.
4.  **Acción 2: Publicar en MQTT**:
    *   Publica un resumen de la información (ej: temperatura actual, probabilidad de lluvia) en un tema MQTT que siga la estructura: `<tu_nombre_minusculas>/meteo/resumen`.

### Rama 2: Reacción a Comandos por MQTT

Esta rama escuchará órdenes y actuará en consecuencia.

1.  **Disparador (Trigger)**: Usa un nodo **MQTT Trigger** para suscribirte a un tema de comandos.
    *   El tema debe seguir el formato: `<tu_nombre_minusculas>/command/email`.
2.  **Acción**: Cuando recibas un mensaje en ese tema, el flujo debe:
    *   Enviar un correo electrónico al profesor (`felipe@uma.es`).
    *   El correo debe ser enviado **desde tu propia cuenta de correo de la UMA**.
    *   El **asunto** del correo debe ser `[AC] Mensaje desde n8n - <Tu Nombre>`.
    *   El **cuerpo** del correo debe contener el `payload` del mensaje MQTT que ha disparado el flujo.

---

## ✨ Ideas para Mejorar tu Proyecto (Opcional)

¿Quieres ir un paso más allá? Aquí tienes algunas ideas para hacer tu proyecto más interesante:

*   **Otras Fuentes de Datos**: En lugar de AEMET, puedes usar otras APIs como Open-Meteo (no requiere clave), datos de calidad del aire, precios de criptomonedas, o incluso resultados deportivos.
*   **Uso de IA**: Añade un nodo **OpenAI** o similar. Por ejemplo, puedes pasarle los datos brutos de la AEMET y pedirle que genere un texto descriptivo y amigable como: *"Hoy en Málaga se espera un día soleado con una temperatura máxima de 28°C. La probabilidad de lluvia es baja."*. Luego, publica este texto por MQTT.
*   **Lógica Condicional**: Utiliza nodos **IF** para tomar decisiones. Por ejemplo: "Si la probabilidad de lluvia es mayor al 70%, publica un mensaje de alerta en el topic `<tu_nombre_minusculas>/meteo/alerta_lluvia`".
*   **Base de Datos más Compleja**: Diseña un esquema con varias tablas relacionadas en lugar de una sola.

---

## 📦 Entrega

1.  **Exporta tu flujo de trabajo** desde n8n. Esto generará un archivo `.json`.
2.  Nombra el archivo como `entrega2_<tu_nombre>_<tu_apellido>.json`.
3.  Envía el archivo por correo electrónico al profesor.

El profesor podrá verificar el correcto funcionamiento consultando la base de datos a través de `phpMyAdmin` y monitorizando los temas MQTT con `MQTT Explorer`.

