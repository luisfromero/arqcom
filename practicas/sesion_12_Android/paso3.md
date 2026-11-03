# Paso 3 – Usar las herramientas: HTTP, JSON y MQTT

Ahora la app va a hacer tres cosas al arrancar:

1. **HTTP + JSON:** descargar `https://casium.uma.es/hola.json` y mostrar el
   valor de la clave `mensaje`.
2. **MQTT (publicar):** conectarse al broker `casium.uma.es`, puerto `8883`, y
   publicar en el tema `narjis/android` que la app se ha iniciado.
3. **MQTT (suscribir):** escuchar el tema `narjis/android/mensajes` y mostrar en
   pantalla el último mensaje que llegue.

Solo hay que cambiar **un fichero**: `MainActivity.kt`.

---

## 1. Lo que vamos a descargar

Si abres <https://casium.uma.es/hola.json> en el navegador, verás:

```json
{"mensaje":"Hola remoto"}
```

Es un objeto JSON con una clave, `mensaje`, cuyo valor es `"Hola remoto"`.
Ese texto es el que mostraremos.

---

## 2. Escribir `MainActivity.kt`

Abre `MainActivity.kt`, **borra todo** su contenido y pega esto:

```kotlin
package com.helioserver.myapplication

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.lifecycle.lifecycleScope
import com.helioserver.myapplication.herramientas.HttpHelper
import com.helioserver.myapplication.herramientas.JsonHelper
import com.helioserver.myapplication.herramientas.MqttHelper
import com.helioserver.myapplication.ui.theme.MyApplicationTheme
import kotlinx.coroutines.launch

class MainActivity : ComponentActivity() {

    // Textos que se ven en pantalla. Al cambiarlos, la pantalla se actualiza sola.
    private var textoHttp by mutableStateOf("Descargando...")
    private var textoMqtt by mutableStateOf("Esperando mensajes...")

    // Cliente MQTT. "ssl://" porque el puerto 8883 usa conexión segura.
    private val mqtt = MqttHelper("ssl://casium.uma.es:8883")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            MyApplicationTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    Pantalla(
                        textoHttp = textoHttp,
                        textoMqtt = textoMqtt,
                        modifier = Modifier.padding(innerPadding)
                    )
                }
            }
        }

        // Lanzamos las dos tareas de red en segundo plano
        lifecycleScope.launch { descargarJson() }
        lifecycleScope.launch { usarMqtt() }
    }

    // HTTP + JSON: descarga hola.json y muestra el valor de "mensaje"
    private suspend fun descargarJson() {
        try {
            val texto = HttpHelper.get("https://casium.uma.es/hola.json")
            val json = JsonHelper.leerObjeto(texto)
            textoHttp = json.getString("mensaje")
        } catch (e: Exception) {
            textoHttp = "Error: ${e.message}"
        }
    }

    // MQTT: conecta, se suscribe y publica que la app se ha iniciado
    private suspend fun usarMqtt() {
        try {
            mqtt.conectar()
            mqtt.suscribir("narjis/android/mensajes") { _, mensaje ->
                textoMqtt = mensaje
            }
            mqtt.publicar("narjis/android", "Se ha iniciado la app")
        } catch (e: Exception) {
            textoMqtt = "Error: ${e.message}"
        }
    }
}

@Composable
fun Pantalla(textoHttp: String, textoMqtt: String, modifier: Modifier = Modifier) {
    Column(modifier = modifier.padding(16.dp)) {
        Text("Mensaje HTTP:")
        Text(textoHttp, style = MaterialTheme.typography.headlineSmall)

        Spacer(Modifier.height(24.dp))

        Text("Último mensaje MQTT:")
        Text(textoMqtt, style = MaterialTheme.typography.headlineSmall)
    }
}

@Preview(showBackground = true)
@Composable
fun PantallaPreview() {
    MyApplicationTheme {
        Pantalla(textoHttp = "Hola remoto", textoMqtt = "Esperando mensajes...")
    }
}
```

---

## 3. Qué hace cada parte

### Los textos de la pantalla

```kotlin
private var textoHttp by mutableStateOf("Descargando...")
private var textoMqtt by mutableStateOf("Esperando mensajes...")
```

`mutableStateOf` crea una variable "vigilada": cada vez que le das un valor
nuevo, Compose **vuelve a dibujar** la pantalla. Por eso basta con escribir
`textoHttp = "..."` para que cambie lo que se ve.

### Lanzar el trabajo de red

```kotlin
lifecycleScope.launch { descargarJson() }
lifecycleScope.launch { usarMqtt() }
```

`lifecycleScope.launch` ejecuta el código **sin bloquear la pantalla**. Las dos
tareas van a la vez y por separado: si una falla, la otra sigue funcionando.

### HTTP + JSON

```kotlin
val texto = HttpHelper.get("https://casium.uma.es/hola.json")  // 1. descargar
val json = JsonHelper.leerObjeto(texto)                        // 2. convertir a JSON
textoHttp = json.getString("mensaje")                          // 3. sacar la clave
```

### MQTT

```kotlin
mqtt.conectar()                                           // 1. conectar al broker
mqtt.suscribir("narjis/android/mensajes") { _, mensaje -> // 2. escuchar un tema
    textoMqtt = mensaje                                   //    y mostrar lo que llegue
}
mqtt.publicar("narjis/android", "Se ha iniciado la app")  // 3. avisar de que arrancamos
```

> **¿Por qué `ssl://`?** El puerto `8883` es el de MQTT **seguro** (con cifrado
> TLS). Si fuera el puerto `1883` (sin cifrar), usaríamos `tcp://`.

### `try` / `catch`

Si algo falla (no hay Internet, el servidor no responde...), en vez de cerrarse
la app se muestra `Error: ...` en pantalla.

---

## 4. Ejecutar

Pulsa ▶ **Run 'app'**. Al cabo de un momento deberías ver:

```
Mensaje HTTP:
Hola remoto

Último mensaje MQTT:
Esperando mensajes...
```

---

## 5. Probar MQTT

Necesitas un cliente MQTT en el ordenador, por ejemplo
[MQTT Explorer](https://mqtt-explorer.com/) o `mosquitto_pub` / `mosquitto_sub`.

**Ver que la app publica al arrancar:**

```bash
mosquitto_sub -h casium.uma.es -p 8883 --capath /etc/ssl/certs -t "narjis/android"
```

Arranca la app y verás aparecer `Se ha iniciado la app`.

**Enviar un mensaje a la app:**

```bash
mosquitto_pub -h casium.uma.es -p 8883 --capath /etc/ssl/certs -t "narjis/android/mensajes" -m "Hola desde el PC"
```

El texto aparecerá en la pantalla del móvil, debajo de *Último mensaje MQTT*.

> En Windows, en lugar de `--capath /etc/ssl/certs` puede ser más fácil usar
> MQTT Explorer: host `casium.uma.es`, puerto `8883` y la opción **TLS** activada.

---

## Problemas frecuentes

| Lo que ves | Causa probable |
|---|---|
| `Error: Unable to resolve host...` | El móvil o el emulador no tiene Internet |
| `Error: ...` en la parte MQTT | El broker no responde, o has puesto `tcp://` en vez de `ssl://` |
| Se queda en `Descargando...` | La conexión es lenta. Espera unos segundos |
| Al girar el móvil vuelve a publicar *Se ha iniciado la app* | Es normal: al girar la pantalla Android reinicia la actividad |

---

## Resumen

| Herramienta | Línea clave |
|---|---|
| HTTP | `HttpHelper.get(url)` |
| JSON | `JsonHelper.leerObjeto(texto).getString("mensaje")` |
| MQTT publicar | `mqtt.publicar(tema, mensaje)` |
| MQTT suscribir | `mqtt.suscribir(tema) { _, mensaje -> ... }` |
