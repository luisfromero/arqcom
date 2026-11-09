# Paso 6: Publicar en el topic de cada melodía

Al pulsar un botón se publica por MQTT el nombre de la melodía en su topic.

## En `Pantalla`

Cambiar la cabecera por:

```kotlin
fun Pantalla(textoHttp: String, textoMqtt: String, melodias: List<Pair<String, String>> = emptyList(), onMelodia: (String, String) -> Unit = { _, _ -> }, modifier: Modifier = Modifier) {
```

En el botón, cambiar `onClick = { }` por:

```kotlin
onClick = { onMelodia(topic, nombre) }
```

## En `onCreate`

En la llamada a `Pantalla`, añadir después de `melodias = melodias,`:

```kotlin
onMelodia = { topic, nombre -> lifecycleScope.launch {
    try { mqtt.publicar(topic, nombre) } catch (e: Exception) { textoMqtt = "Error: ${e.message}" }
} },
```

El `try/catch` evita que la app se cierre si se pulsa un botón antes de que MQTT haya conectado.

## Corrección en `usarMqtt()`

Al topic le faltaba la comilla de cierre:

```kotlin
mqtt.publicar("android/narjis/estado", "Se ha iniciado la app")
```
