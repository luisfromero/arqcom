# Paso 4: Descargar las melodías

Se sustituye la descarga de `hola.json` por la de `melodias.json`, y se extraen el número de melodías y una lista de pares `(nombre, topic)`.

## En la clase, junto a `textoHttp`

Añadir:

```kotlin
var numMelodias = 0
var melodias = listOf<Pair<String, String>>()   // (nombre, topic)
```

## En `descargarJson()`

Cambiar la URL:

```kotlin
val texto = HttpHelper.get("https://casium.uma.es/apps/melodias/melodias.json")
```

Eliminar:

```kotlin
textoHttp = json.getString("mensaje")
```

Añadir en su lugar:

```kotlin
val array = json.getJSONArray("melodias")
numMelodias = array.length()
melodias = List(numMelodias) { i ->
    array.getJSONObject(i).let { it.getString("nombre") to it.optString("topic", "") }
}
textoHttp = "$numMelodias melodías:\n" + melodias.joinToString("\n") { it.first }
```

## Resultado

```kotlin
private suspend fun descargarJson() {
    try {
        val texto = HttpHelper.get("https://casium.uma.es/apps/melodias/melodias.json")
        val json = JsonHelper.leerObjeto(texto)
        val array = json.getJSONArray("melodias")
        numMelodias = array.length()
        melodias = List(numMelodias) { i ->
            array.getJSONObject(i).let { it.getString("nombre") to it.optString("topic", "") }
        }
        textoHttp = "$numMelodias melodías:\n" + melodias.joinToString("\n") { it.first }
    } catch (e: Exception) {
        textoHttp = "Error: ${e.message}"
    }
}
```

Para acceder a los datos: `melodias[i].first` es el nombre y `melodias[i].second` es el topic.
