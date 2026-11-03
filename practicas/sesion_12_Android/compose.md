# Mini-tutorial de Jetpack Compose

> **Jetpack Compose** es la forma moderna (y recomendada por Google) de crear la interfaz
> de una app Android. La pantalla se escribe en **Kotlin**, con funciones que describen
> lo que se ve. Si aún no te manejas con Kotlin, empieza por [kotlin.md](kotlin.md).

Volver al [README](readme.MD)

---

## 1. La idea en una frase

> **Describes cómo debe verse la pantalla para un estado dado, y Compose se encarga de
> dibujarla y de redibujarla cuando ese estado cambia.**

```kotlin
@Composable
fun Saludo(nombre: String) {
    Text("Hola, $nombre")
}
```

- `@Composable` marca una función que **dibuja interfaz**.
- Por convención, su nombre empieza por **mayúscula** (`Saludo`, no `saludo`).
- No devuelve nada: simplemente "emite" elementos en pantalla.
- Solo se puede llamar a una función `@Composable` desde **otra** función `@Composable`
  (o desde `setContent { … }`).

---

## 2. Vista previa (`@Preview`)

### Para qué sirve
Arrancar el emulador cada vez que cambias un color es lento. La **vista previa** dibuja
un trozo de pantalla **dentro del propio Android Studio**, al lado del código, sin
ejecutar la app. Se ve en la pestaña **Split** (o **Design**), arriba a la derecha del editor.

### El caso más fácil: una función sin parámetros
Si tu función no necesita datos de fuera, basta con ponerle **encima** la etiqueta
`@Preview`, además de `@Composable`:

```kotlin
@Preview
@Composable
fun Tarjeta() {
    Text("Ana García")
}
```

No hay que redefinir nada: es la **misma** función que usa la app, con una etiqueta más.
`@Preview` no cambia lo que hace la función; solo le dice a Android Studio "dibuja esto en la vista previa".

### El problema: funciones con parámetros
Ahora fíjate en esta otra:

```kotlin
@Composable
fun Saludo(nombre: String) {
    Text("Hola, $nombre")
}
```

Para dibujarla hace falta saber **qué nombre** poner. Cuando corre la app, el nombre se lo
pasa quien la llama (por ejemplo `Saludo("Android")` dentro de `setContent`). Pero la vista
previa no ejecuta la app, así que **no sabe qué nombre usar**. Por eso no puedes poner
`@Preview` encima de `Saludo` directamente: Android Studio se queja.

### La solución: una función "de ensayo"
Escribes una función **nueva y pequeña**, sin parámetros, cuyo único trabajo es
**llamar** a la tuya con unos datos de ejemplo:

```kotlin
// 1) Tu función de verdad (la que usa la app). NO se toca.
@Composable
fun Saludo(nombre: String) {
    Text("Hola, $nombre")
}

// 2) Función solo para la vista previa: llama a Saludo con un nombre de ejemplo.
@Preview(showBackground = true)
@Composable
fun SaludoPreview() {
    Saludo("Mundo")      // ← no repite el código de Saludo, solo lo LLAMA
}
```

Piensa en `SaludoPreview` como en un **ensayo**: no vuelves a escribir la obra, solo
la representas con actores de prueba ("Mundo"). Si cambias `Saludo`, la vista previa
cambia sola.

- El nombre `SaludoPreview` es una costumbre; podría llamarse como quieras.
- Esta función **no la usa la app**: solo existe para Android Studio.
- `showBackground = true` pinta un fondo blanco (si no, sale transparente).
- Puedes tener **varias** vistas previas a la vez para probar casos distintos:

```kotlin
@Preview(showBackground = true)
@Composable
fun SaludoCortoPreview() { Saludo("Eva") }

@Preview(showBackground = true)
@Composable
fun SaludoLargoPreview() { Saludo("María de los Ángeles Fernández") }
```

> **Si no se actualiza**: pulsa el botón de refrescar (*Build & Refresh*) del panel de
> vista previa. Después de cambios grandes a veces hay que compilar una vez.

---

## 3. Piezas básicas

```kotlin
Text("Un texto")
Text("Grande y en negrita", fontSize = 24.sp, fontWeight = FontWeight.Bold)

Button(onClick = { /* qué hacer al pulsar */ }) {
    Text("Púlsame")
}

Image(
    painter = painterResource(R.drawable.mi_foto),   // imagen en res/drawable/mi_foto.png
    contentDescription = "Foto de perfil"             // para lectores de pantalla
)

Icon(Icons.Default.Favorite, contentDescription = "Me gusta")

Spacer(Modifier.height(16.dp))                        // hueco vacío
```

> **Unidades**: `dp` para tamaños y márgenes; `sp` para tamaños de letra.
> Así la app se ve igual en móviles con distinta resolución.

---

## 4. Colocar cosas: `Column`, `Row`, `Box`

| Contenedor | Coloca los hijos… |
|-----------|-------------------|
| `Column` | uno **debajo** de otro ↓ |
| `Row` | uno **al lado** de otro → |
| `Box` | uno **encima** de otro (capas) |

```kotlin
Column(
    modifier = Modifier.fillMaxSize().padding(16.dp),
    verticalArrangement = Arrangement.Center,          // centrado vertical
    horizontalAlignment = Alignment.CenterHorizontally // centrado horizontal
) {
    Text("Arriba")
    Row {
        Text("Izquierda")
        Spacer(Modifier.width(8.dp))
        Text("Derecha")
    }
    Text("Abajo")
}
```

Resultado:
```
        Arriba
  Izquierda  Derecha
        Abajo
```

### Modifier: el "ajuste fino"
Casi todos los elementos aceptan un `modifier` para cambiar tamaño, márgenes, fondo,
bordes, que se pueda pulsar, etc. Se **encadenan** con puntos y **el orden importa**:

```kotlin
Text(
    "Etiqueta",
    modifier = Modifier
        .padding(8.dp)                         // margen exterior
        .background(Color.Yellow)              // fondo
        .padding(16.dp)                        // margen interior (después del fondo)
        .clickable { /* al tocar */ }
)
```

Los más usados: `fillMaxSize()`, `fillMaxWidth()`, `size(…)`, `padding(…)`,
`background(…)`, `clickable { … }`, `weight(1f)` (dentro de `Row`/`Column`, para repartir espacio).

---

## 5. Campos de texto

```kotlin
@Composable
fun PedirNombre() {
    var nombre by remember { mutableStateOf("") }

    Column(Modifier.padding(16.dp)) {
        OutlinedTextField(
            value = nombre,                     // lo que se muestra
            onValueChange = { nombre = it },    // qué hacer cuando el usuario escribe
            label = { Text("Tu nombre") }
        )
        Text("Hola, $nombre")
    }
}
```

Fíjate: el campo **no guarda el texto por sí mismo**. Tú le das el valor (`value`) y,
cuando el usuario teclea, te avisa (`onValueChange`) para que actualices el estado.
Esto se llama **flujo de datos unidireccional**: el dato baja, los eventos suben.

---

## 6. Estado: la clave de todo

```kotlin
var contador by remember { mutableStateOf(0) }
```

| Pieza | Significado |
|-------|-------------|
| `mutableStateOf(0)` | una "caja" con valor inicial `0` que Compose **vigila** |
| `remember { … }` | no la vuelvas a crear cada vez que redibujes |
| `by` | permite usar `contador` directamente en vez de `contador.value` |
| `rememberSaveable { … }` | como `remember`, pero sobrevive a girar el móvil |

Cuando el valor cambia ⇒ Compose vuelve a ejecutar las funciones que lo usan
(**recomposición**) ⇒ la pantalla se actualiza.

❌ **Error típico**:
```kotlin
var contador = 0                     // variable normal: Compose no se entera
Button(onClick = { contador++ }) { Text("$contador") }   // siempre muestra 0
```

### Pasar estado a otras funciones (*state hoisting*)
Es buena práctica que las piezas pequeñas **no** tengan estado propio: lo reciben y
avisan cuando cambia. Así se pueden reutilizar.

```kotlin
@Composable
fun BotonContador(valor: Int, alPulsar: () -> Unit) {
    Button(onClick = alPulsar) { Text("Pulsado $valor veces") }
}

@Composable
fun Pantalla() {
    var n by remember { mutableStateOf(0) }
    BotonContador(valor = n, alPulsar = { n++ })
}
```

`() -> Unit` es el **tipo** de una lambda sin parámetros que no devuelve nada.

---

## 7. Listas largas: `LazyColumn`

Para listas que pueden ser largas, `LazyColumn` solo dibuja lo que cabe en pantalla.

```kotlin
@Composable
fun ListaCompra() {
    var texto by remember { mutableStateOf("") }
    val items = remember { mutableStateListOf<String>() }   // lista que Compose vigila

    Column(Modifier.padding(16.dp)) {
        Row(verticalAlignment = Alignment.CenterVertically) {
            OutlinedTextField(
                value = texto,
                onValueChange = { texto = it },
                modifier = Modifier.weight(1f)
            )
            Button(onClick = {
                if (texto.isNotBlank()) { items.add(texto); texto = "" }
            }) { Text("Añadir") }
        }
        LazyColumn {
            items(items) { producto ->
                Text("• $producto", modifier = Modifier.padding(8.dp))
            }
        }
    }
}
```

---

## 8. Mostrar u ocultar cosas

Como la pantalla es código, basta con un `if`:

```kotlin
var mostrar by remember { mutableStateOf(false) }

Column {
    Button(onClick = { mostrar = !mostrar }) {
        Text(if (mostrar) "Ocultar" else "Mostrar")
    }
    if (mostrar) {
        Text("¡Sorpresa! 🎉")
    }
}
```

Con esta misma idea puedes hacer una app de "varias pantallas" muy sencilla:

```kotlin
var pantalla by remember { mutableStateOf("inicio") }
when (pantalla) {
    "inicio"  -> Inicio(irAPerfil = { pantalla = "perfil" })
    "perfil"  -> Perfil(volver = { pantalla = "inicio" })
}
```

(Para apps reales se usa la librería **Navigation Compose**.)

---

## 9. Tema y Material Design

El proyecto trae un tema en `ui/theme/`. Úsalo en lugar de poner colores "a mano":

```kotlin
Text("Título", style = MaterialTheme.typography.headlineMedium)
Text("Aviso", color = MaterialTheme.colorScheme.error)
```

Así, si cambias los colores en `Color.kt` / `Theme.kt`, cambia toda la app, y el modo
oscuro funciona solo.

Componentes Material 3 útiles: `Card`, `Scaffold` (estructura con barra superior y
botón flotante), `TopAppBar`, `FloatingActionButton`, `Checkbox`, `Switch`, `Slider`,
`AlertDialog`.

---

## 10. Chuleta rápida

| Quiero… | Compose |
|---------|---------|
| Texto | `Text("…")` |
| Botón | `Button(onClick = { … }) { Text("…") }` |
| Campo de texto | `OutlinedTextField(value, onValueChange)` |
| Imagen | `Image(painterResource(R.drawable.x), "desc")` |
| Uno debajo de otro | `Column { … }` |
| Uno al lado de otro | `Row { … }` |
| Superpuestos | `Box { … }` |
| Lista larga | `LazyColumn { items(lista) { … } }` |
| Margen | `Modifier.padding(16.dp)` |
| Ocupar todo | `Modifier.fillMaxSize()` |
| Dato que cambia | `var x by remember { mutableStateOf(…) }` |
| Que sobreviva al giro | `rememberSaveable { … }` |

> **Imports**: si algo sale en rojo (`dp`, `Modifier`, `getValue`…), pon el cursor encima
> y pulsa `Alt + Enter` ▸ *Import*. Con `by remember` a veces hay que importar
> `androidx.compose.runtime.getValue` y `setValue`.

---

## Ejercicios

1. **Tarjeta de visita**: foto, nombre, profesión y una fila con teléfono y correo.
2. **Dado**: botón "Tirar" que muestra un número aleatorio del 1 al 6 (`(1..6).random()`).
3. **Calculadora de propina**: campo para el importe, `Slider` para el porcentaje y un
   texto con el total.
4. **Interruptor**: un `Switch` que cambia el color de fondo de la pantalla.
5. **Lista de tareas**: como la lista de la compra, pero con un `Checkbox` en cada fila.
