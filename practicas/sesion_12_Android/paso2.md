# Paso 2 – Preparar las herramientas básicas

En este paso **solo preparamos** la app para poder usar:

| Herramienta | Para qué sirve | Librería |
|---|---|---|
| **HTTP / HTTPS** | Pedir datos a una web o API (como `curl`) | OkHttp |
| **JSON** | Leer y construir datos en formato JSON | `org.json` (ya viene en Android) |
| **MQTT** | Publicar y recibir mensajes de un broker | Eclipse Paho |

Todavía **no probamos** nada. Eso lo haremos en el paso 3.

Al terminar, la app debe compilar y ejecutarse igual que en el paso 1 (sigue mostrando "Hello Android!").

---

## 1. Añadir las librerías al catálogo de versiones

Abre `gradle/libs.versions.toml`.

En la sección `[versions]`, añade al final:

```toml
# Paso 2: herramientas de red
okhttp = "5.5.1"
pahoMqtt = "1.2.5"
coroutines = "1.10.2"
```

En la sección `[libraries]`, añade al final:

```toml
# Paso 2: herramientas de red
okhttp = { group = "com.squareup.okhttp3", name = "okhttp", version.ref = "okhttp" }
paho-mqtt = { group = "org.eclipse.paho", name = "org.eclipse.paho.client.mqttv3", version.ref = "pahoMqtt" }
kotlinx-coroutines-android = { group = "org.jetbrains.kotlinx", name = "kotlinx-coroutines-android", version.ref = "coroutines" }
```

> **¿Y las corrutinas?** En Android está prohibido usar la red desde el hilo
> principal (el de la pantalla). Las corrutinas permiten mandar ese trabajo a
> segundo plano de forma sencilla.

---

## 2. Usar las librerías en la app

Abre `app/build.gradle.kts` y, dentro de `dependencies { ... }`, justo después de
la línea `implementation(libs.androidx.lifecycle.runtime.ktx)`, añade:

```kotlin
    // Paso 2: herramientas de red
    implementation(libs.okhttp)                     // HTTP/HTTPS (como curl)
    implementation(libs.paho.mqtt)                  // MQTT (publicar / suscribir)
    implementation(libs.kotlinx.coroutines.android) // tareas en segundo plano
    // JSON: no hace falta dependencia, Android ya incluye org.json
```

Ahora pulsa **Sync Now** (barra amarilla de arriba) o el icono del elefante
🐘 **Sync Project with Gradle Files**.

---

## 3. Dar permiso de Internet

Abre `app/src/main/AndroidManifest.xml`.

1. Justo antes de `<application`, añade el permiso:

   ```xml
   <!-- Paso 2: permiso para usar Internet (HTTP y MQTT) -->
   <uses-permission android:name="android.permission.INTERNET" />
   ```

2. Dentro de la etiqueta `<application`, añade este atributo:

   ```xml
   android:usesCleartextTraffic="true"
   ```

   Así funcionan también las direcciones `http://` (sin la "s"). Por defecto
   Android solo deja usar `https://`. Es práctico para clase y para servidores
   locales. En una app real conviene usar solo `https://`.

El principio del manifest queda así:

```xml
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:tools="http://schemas.android.com/tools">

    <!-- Paso 2: permiso para usar Internet (HTTP y MQTT) -->
    <uses-permission android:name="android.permission.INTERNET" />

    <application
        android:usesCleartextTraffic="true"
        android:allowBackup="true"
        ...
```

---

## 4. Crear el paquete `herramientas`

En el panel *Project*, haz clic derecho sobre el paquete
`com.helioserver.myapplication` → **New → Package**, y escribe `herramientas`.

Dentro de ese paquete crearemos tres ficheros Kotlin
(clic derecho → **New → Kotlin Class/File**).

```
app/src/main/java/com/helioserver/myapplication/
├── MainActivity.kt
├── herramientas/
│   ├── HttpHelper.kt   ← HTTP/HTTPS
│   ├── JsonHelper.kt   ← JSON
│   └── MqttHelper.kt   ← MQTT
└── ui/theme/...
```

---

### 4.1 `HttpHelper.kt`

```kotlin
package com.helioserver.myapplication.herramientas

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import java.io.IOException

/**
 * Peticiones HTTP/HTTPS sencillas (como "curl").
 *
 * Las funciones son "suspend": se llaman desde una corrutina
 * (por ejemplo, dentro de lifecycleScope.launch { ... }).
 * El trabajo de red se hace en segundo plano (Dispatchers.IO).
 */
object HttpHelper {

    private val cliente = OkHttpClient()

    /** Hace un GET y devuelve el cuerpo de la respuesta como texto. */
    suspend fun get(url: String): String = withContext(Dispatchers.IO) {
        val peticion = Request.Builder()
            .url(url)
            .build()
        ejecutar(peticion)
    }

    /** Hace un POST enviando un texto JSON y devuelve la respuesta como texto. */
    suspend fun postJson(url: String, json: String): String = withContext(Dispatchers.IO) {
        val cuerpo = json.toRequestBody("application/json; charset=utf-8".toMediaType())
        val peticion = Request.Builder()
            .url(url)
            .post(cuerpo)
            .build()
        ejecutar(peticion)
    }

    private fun ejecutar(peticion: Request): String {
        cliente.newCall(peticion).execute().use { respuesta ->
            if (!respuesta.isSuccessful) {
                throw IOException("Error HTTP ${respuesta.code}")
            }
            return respuesta.body.string()
        }
    }
}
```

| Función | Equivale a |
|---|---|
| `HttpHelper.get(url)` | `curl https://...` |
| `HttpHelper.postJson(url, json)` | `curl -X POST -H "Content-Type: application/json" -d '{...}' https://...` |

---

### 4.2 `JsonHelper.kt`

```kotlin
package com.helioserver.myapplication.herramientas

import org.json.JSONArray
import org.json.JSONObject

/**
 * Leer y construir JSON usando org.json (ya viene incluido en Android).
 */
object JsonHelper {

    /**
     * Construye un objeto JSON a partir de parejas clave-valor.
     * Ejemplo: JsonHelper.crear("nombre" to "Ana", "edad" to 20)
     *          -> {"nombre":"Ana","edad":20}
     */
    fun crear(vararg pares: Pair<String, Any?>): String {
        val objeto = JSONObject()
        for ((clave, valor) in pares) {
            objeto.put(clave, valor)
        }
        return objeto.toString()
    }

    /** Convierte un texto con un objeto JSON {...} en un JSONObject. */
    fun leerObjeto(texto: String): JSONObject = JSONObject(texto)

    /** Convierte un texto con una lista JSON [...] en un JSONArray. */
    fun leerLista(texto: String): JSONArray = JSONArray(texto)
}
```

| Función | Qué hace |
|---|---|
| `JsonHelper.crear("a" to 1, "b" to "hola")` | Devuelve el texto `{"a":1,"b":"hola"}` |
| `JsonHelper.leerObjeto(texto)` | Devuelve un `JSONObject`; luego usas `.getString("clave")`, `.getInt("clave")`... |
| `JsonHelper.leerLista(texto)` | Devuelve un `JSONArray`; luego usas `.length()` y `.getJSONObject(i)` |

---

### 4.3 `MqttHelper.kt`

```kotlin
package com.helioserver.myapplication.herramientas

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import org.eclipse.paho.client.mqttv3.IMqttMessageListener
import org.eclipse.paho.client.mqttv3.MqttClient
import org.eclipse.paho.client.mqttv3.MqttConnectOptions
import org.eclipse.paho.client.mqttv3.MqttMessage
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence
import java.util.UUID

/**
 * Cliente MQTT sencillo: conectar, publicar, suscribir y desconectar.
 *
 * @param servidor dirección del broker, por ejemplo "tcp://broker.hivemq.com:1883"
 * @param idCliente identificador único de este cliente (se genera uno al azar)
 *
 * Las funciones son "suspend": se llaman desde una corrutina
 * (por ejemplo, dentro de lifecycleScope.launch { ... }).
 */
class MqttHelper(
    servidor: String = "tcp://broker.hivemq.com:1883",
    idCliente: String = "android-" + UUID.randomUUID().toString().take(8)
) {

    // MemoryPersistence: guarda el estado en memoria (no en ficheros)
    private val cliente = MqttClient(servidor, idCliente, MemoryPersistence())

    /** Conecta con el broker. */
    suspend fun conectar() {
        withContext(Dispatchers.IO) {
            val opciones = MqttConnectOptions().apply {
                isCleanSession = true
                isAutomaticReconnect = true
            }
            cliente.connect(opciones)
        }
    }

    /** Publica un mensaje de texto en un tema (topic). */
    suspend fun publicar(tema: String, mensaje: String) {
        withContext(Dispatchers.IO) {
            cliente.publish(tema, MqttMessage(mensaje.toByteArray()))
        }
    }

    /**
     * Se suscribe a un tema. Cada vez que llegue un mensaje
     * se llama a [alRecibir] con el tema y el texto del mensaje.
     */
    suspend fun suscribir(tema: String, alRecibir: (tema: String, mensaje: String) -> Unit) {
        withContext(Dispatchers.IO) {
            cliente.subscribe(tema, IMqttMessageListener { temaRecibido, mensaje ->
                alRecibir(temaRecibido, String(mensaje.payload))
            })
        }
    }

    /** Indica si estamos conectados al broker. */
    fun estaConectado(): Boolean = cliente.isConnected

    /** Se desconecta del broker. */
    suspend fun desconectar() {
        withContext(Dispatchers.IO) {
            if (cliente.isConnected) {
                cliente.disconnect()
            }
        }
    }
}
```

| Función | Qué hace |
|---|---|
| `MqttHelper()` | Crea el cliente (por defecto usa el broker público `broker.hivemq.com`) |
| `conectar()` | Se conecta al broker |
| `publicar(tema, mensaje)` | Envía un mensaje a un tema |
| `suscribir(tema) { tema, mensaje -> ... }` | Escucha un tema y ejecuta el código cada vez que llega un mensaje |
| `desconectar()` | Cierra la conexión |

> **Nota:** `broker.hivemq.com` es un broker **público**: cualquiera puede leer
> lo que publiques. Usa temas con un nombre único (por ejemplo
> `clase/tu-nombre/prueba`) y no envíes datos personales.

---

## 5. Comprobar que todo compila

1. Menú **Build → Assemble Project** (o el martillo 🔨).
2. Debe terminar con **BUILD SUCCESSFUL**.
3. Ejecuta la app (▶). Debe verse igual que en el paso 1: "Hello Android!".

Si sale un error tipo *Unresolved reference okhttp3* o *paho*, casi siempre es
porque falta sincronizar Gradle: pulsa 🐘 **Sync Project with Gradle Files** y
vuelve a compilar.

---

## Resumen de lo que hemos cambiado

| Fichero | Cambio |
|---|---|
| `gradle/libs.versions.toml` | Versiones y nombres de OkHttp, Paho MQTT y corrutinas |
| `app/build.gradle.kts` | 3 líneas `implementation(...)` |
| `app/src/main/AndroidManifest.xml` | Permiso `INTERNET` y `usesCleartextTraffic` |
| `herramientas/HttpHelper.kt` | Nuevo: GET y POST |
| `herramientas/JsonHelper.kt` | Nuevo: crear y leer JSON |
| `herramientas/MqttHelper.kt` | Nuevo: conectar, publicar, suscribir, desconectar |

`MainActivity.kt` **no se ha tocado**. En el paso 3 usaremos estas herramientas.
