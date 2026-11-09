# Paso 5: Un botón por melodía

Se añade a `Pantalla` un botón vertical por cada melodía descargada.

## En la clase

Para que la pantalla se redibuje al terminar la descarga, `melodias` pasa a ser estado de Compose.

Eliminar:

```kotlin
var melodias = listOf<Pair<String, String>>()   // (nombre, topic)
```

Añadir:

```kotlin
var melodias by mutableStateOf(listOf<Pair<String, String>>())   // (nombre, topic)
```

## En `onCreate`

En la llamada a `Pantalla`, añadir entre `textoMqtt` y `modifier`:

```kotlin
melodias = melodias,
```

## En `Pantalla`

Cambiar la cabecera por:

```kotlin
fun Pantalla(textoHttp: String, textoMqtt: String, melodias: List<Pair<String, String>> = emptyList(), modifier: Modifier = Modifier) {
```

Dentro del `Column`, después de `Text(textoMqtt, ...)`, añadir:

```kotlin
melodias.forEach { (nombre, topic) ->
    Button(onClick = { }, modifier = Modifier.fillMaxWidth()) {
        Text(nombre)
    }
}
```

Gracias al valor por defecto `emptyList()`, `PantallaPreview` no necesita cambios.

## Imports

Añadir solo los que falten:

```kotlin
import androidx.compose.material3.Button
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.compose.runtime.mutableStateOf
```
