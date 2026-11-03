# Sesión 12 · Introducción a Android con Kotlin y Jetpack Compose

> **¿Para quién es esto?** Para quien apenas ha programado. No hace falta saber Java ni
> haber hecho nunca una app. Iremos paso a paso y, cuando aparezca un concepto nuevo,
> lo explicaremos en una **💊 píldora** corta.

Material de apoyo (léelo antes o tenlo abierto durante la sesión):

| Documento | Qué contiene |
|-----------|--------------|
| [kotlin.md](kotlin.md) | Mini-tutorial de **Kotlin**, el lenguaje que usaremos |
| [compose.md](compose.md) | Mini-tutorial de **Jetpack Compose**, la forma moderna de dibujar pantallas |

---

## Objetivos

Al terminar la sesión deberías ser capaz de:

1. Crear un proyecto en **Android Studio** y ejecutarlo en el emulador o en tu móvil.
2. Leer y entender un programa sencillo en **Kotlin**.
3. Construir una pantalla con **Compose**: textos, botones, campos de texto e imágenes.
4. Hacer que la pantalla **reaccione** a lo que hace el usuario (estado).
5. Saber qué es una **Activity** y su **ciclo de vida**.

---

## Plan de la sesión (≈ 2 horas)

| Bloque | Tiempo | Contenido |
|--------|--------|-----------|
| 1 | 15 min | Android Studio y primer proyecto |
| 2 | 25 min | Kotlin lo justo (ver [kotlin.md](kotlin.md)) |
| 3 | 30 min | Primeras pantallas con Compose (ver [compose.md](compose.md)) |
| 4 | 25 min | Estado: pantallas que cambian |
| 5 | 15 min | Activity y ciclo de vida |
| 6 | 10 min | Depurar: Logcat y errores típicos |

---

## Bloque 1 · Android Studio y primer proyecto

### ¿Qué es Android Studio?
Es el **IDE** (programa para escribir programas) oficial de Android. Incluye:
- un **editor** de código que te avisa de errores mientras escribes,
- un **emulador** (un móvil virtual dentro del PC),
- una **vista previa** (*Preview*) que dibuja tu pantalla sin ejecutar la app.

### Crear el proyecto
1. `File ▸ New ▸ New Project…`
2. Elige **Empty Activity** (el icono con el logo de Compose). ⚠️ *No* elijas "Empty Views Activity": esa es la forma antigua con XML.
3. Nombre: `MiPrimeraApp`. Lenguaje: **Kotlin** (ya viene por defecto). Build configuration: **Kotlin DSL**.
4. Pulsa **Finish** y espera (la primera vez tarda: descarga muchas cosas).
5. Pulsa ▶ **Run**. Debería aparecer "Hello Android!" en el emulador.

### ¿Qué hay dentro del proyecto?

```
app/
 ├─ src/main/
 │   ├─ java/…/MainActivity.kt   ← TU CÓDIGO (sí, en la carpeta "java" aunque sea Kotlin)
 │   ├─ java/…/ui/theme/         ← colores, tipografías y tema de la app
 │   ├─ res/                     ← recursos: iconos, imágenes, textos traducibles
 │   └─ AndroidManifest.xml      ← la "ficha" de la app: nombre, icono, permisos, pantallas
 └─ build.gradle.kts             ← configuración y librerías que usa la app
```

> **💊 Píldora — ¿Qué es Gradle?**
> Gradle es el "cocinero" que coge tu código, las librerías y los recursos y los convierte
> en un fichero `.apk` instalable. Cuando cambies `build.gradle.kts`, Android Studio te
> pedirá **Sync Now**: es Gradle volviendo a leer la receta.

---

## Bloque 2 · Kotlin lo justo

Lee [kotlin.md](kotlin.md). Para esta sesión basta con:

- `val` (no cambia) y `var` (sí cambia).
- Funciones con `fun`.
- `if`, `when` y bucles `for`.
- Clases y **herencia** (ver píldora abajo).
- Lambdas: trozos de código entre llaves `{ … }` que se pasan como dato.

> **💊 Píldora — Herencia de clases**
> Una **clase** es un molde para crear objetos. La **herencia** permite crear un molde
> nuevo *a partir* de otro, reutilizando lo que ya hace y añadiendo o cambiando cosas.
>
> ```kotlin
> open class Animal(val nombre: String) {          // "open" = se puede heredar de ella
>     open fun hablar() = "…"
> }
>
> class Perro(nombre: String) : Animal(nombre) {   // Perro ES UN Animal
>     override fun hablar() = "¡Guau!"              // cambiamos el comportamiento
> }
> ```
>
> **¿Por qué importa en Android?** Tu pantalla principal es:
> ```kotlin
> class MainActivity : ComponentActivity() { … }
> ```
> `MainActivity` **hereda** de `ComponentActivity`, una clase que Google ya ha programado
> y que sabe hablar con el sistema Android. Tú solo **sobrescribes** (`override`) las partes
> que te interesan, como `onCreate`. Así no tienes que programar miles de líneas tú.

> **💊 Píldora — `null` y el signo `?`**
> En Kotlin una variable **no puede** estar vacía (`null`) salvo que lo digas con `?`:
> `var nombre: String? = null`. Esto evita el error más famoso de la programación
> (*NullPointerException*). Más en [kotlin.md](kotlin.md#7-null-safety-el-signo-).

---

## Bloque 3 · Primeras pantallas con Compose

Abre `MainActivity.kt`. Verás algo parecido a esto (simplificado):

```kotlin
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {                 // ← "a partir de aquí, la pantalla la dibuja Compose"
            MiPrimeraAppTheme {      // ← aplica colores y tipografías de la app
                Saludo("Android")
            }
        }
    }
}

@Composable
fun Saludo(nombre: String) {
    Text(text = "Hola $nombre!")
}
```

> **💊 Píldora — ¿Qué es Compose?**
> Antes, las pantallas se dibujaban en ficheros XML aparte y luego había que "conectarlas"
> con el código. Con **Jetpack Compose** la pantalla **es código Kotlin**: escribes
> funciones marcadas con `@Composable` que **describen** lo que se ve.
> Es como escribir una receta: "una columna con un título y debajo un botón".

> **💊 Píldora — Anotaciones (`@Algo`)**
> Una palabra con `@` delante es una **etiqueta** que se pega a una función o clase para
> darle un significado especial. `@Composable` dice "esta función dibuja interfaz";
> `@Preview` dice "enséñame esto en la vista previa".

> **💊 Píldora — ¿Y esa función `GreetingPreview` del final del fichero?**
> La plantilla trae, además de `Greeting(name)`, otra función `GreetingPreview()` con `@Preview`.
> **No es una copia** de `Greeting`: solo la **llama** con un nombre de ejemplo, para que
> Android Studio pueda dibujarla en la vista previa sin arrancar la app. Hace falta porque
> `Greeting` necesita un nombre y la vista previa no sabría cuál poner.
> Explicación completa en [compose.md](compose.md#2-vista-previa-preview).

### Ejercicio 3.1 — Tarjeta de presentación
Cambia `Saludo` por una pantalla con tu nombre, tus estudios y un botón:

```kotlin
@Composable
fun Tarjeta() {
    Column(
        modifier = Modifier.fillMaxSize().padding(24.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text("Ana García", style = MaterialTheme.typography.headlineMedium)
        Text("Grado en Biología")
        Spacer(Modifier.height(16.dp))
        Button(onClick = { /* de momento nada */ }) {
            Text("Contactar")
        }
    }
}
```

Las piezas básicas (`Text`, `Button`, `Column`, `Row`, `Box`, `Image`, `TextField`) y los
`Modifier` están explicados en [compose.md](compose.md).

> **💊 Píldora — Parámetros con nombre**
> `Text(text = "Hola", color = Color.Red)` es lo mismo que `Text("Hola", color = Color.Red)`.
> Poner el nombre (`text =`) hace el código más fácil de leer. En Compose verás mucho esto.

---

## Bloque 4 · Estado: pantallas que cambian

Una pantalla que nunca cambia es un póster. Para que **reaccione**, Compose usa **estado**.

```kotlin
@Composable
fun Contador() {
    var veces by remember { mutableStateOf(0) }   // un dato que Compose "vigila"

    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Text("Has pulsado $veces veces")
        Button(onClick = { veces++ }) {            // al pulsar, cambia el estado…
            Text("¡Púlsame!")
        }
    }
}                                                  // …y Compose redibuja solo lo necesario
```

> **💊 Píldora — Estado y recomposición**
> - `mutableStateOf(0)`: crea una "caja" con un valor que Compose vigila.
> - `remember { … }`: "acuérdate de esta caja aunque se redibuje la pantalla".
> - Cuando el valor cambia, Compose vuelve a ejecutar la función (**recomposición**) y la
>   pantalla se actualiza **sola**. Tú nunca dices "cambia el texto de la etiqueta X";
>   solo cambias el dato.
>
> Regla de oro: **la pantalla es una función del estado**. `UI = f(estado)`.

> **💊 Píldora — Lambdas (`{ … }`)**
> `onClick = { veces++ }` le pasa al botón **un trozo de código** para que lo ejecute
> *cuando* le pulsen, no ahora. Eso es una **lambda**. Ver [kotlin.md](kotlin.md#6-lambdas).

### Ejercicio 4.1 — Saludo personalizado
Un `TextField` donde el usuario escribe su nombre y un `Text` que diga "Hola, <nombre>"
mientras escribe. Pista en [compose.md](compose.md#5-campos-de-texto).

### Ejercicio 4.2 — Conversor de unidades
Un campo para escribir grados Celsius y un texto que muestre los Fahrenheit
(`F = C * 9 / 5 + 32`). Pista: `texto.toDoubleOrNull()` devuelve `null` si no es un número.

### Ejercicio 4.3 (opcional) — Lista de la compra
Un campo de texto + botón "Añadir" que mete elementos en una lista mostrada con
`LazyColumn`. Ver [compose.md](compose.md#7-listas-largas-lazycolumn).

> **💊 Píldora — Girar el móvil borra el estado**
> Si giras el emulador, el contador vuelve a 0. Android **destruye y recrea** la Activity
> al girar. Cambia `remember` por `rememberSaveable` y verás que sobrevive.
> Esto conecta con el siguiente bloque.

---

## Bloque 5 · Activity y ciclo de vida

> **💊 Píldora — ¿Qué es una Activity?**
> Es el "punto de entrada" de una pantalla de tu app: lo que Android abre cuando tocas el
> icono. Con Compose lo normal es tener **una sola Activity** y cambiar lo que se ve dentro
> con estado o con *Navigation Compose*.

Android no deja a tu app hacer lo que quiera: la va **avisando** de lo que pasa
llamando a ciertas funciones. Ese es el **ciclo de vida**:

```
            onCreate()   → se crea (aquí va setContent)
                ↓
            onStart()    → ya es visible
                ↓
            onResume()   → ya puedes tocarla  ✅ app en uso
                ↓
            onPause()    → algo la tapa parcialmente (p. ej. un diálogo)
                ↓
            onStop()     → ya no se ve (fuiste a otra app)
                ↓
            onDestroy()  → se destruye (cerrada, o el móvil se giró)
```

### Ejercicio 5.1 — Espiar el ciclo de vida
Añade a `MainActivity` (es **herencia** otra vez: sobrescribimos métodos del padre):

```kotlin
override fun onStart()   { super.onStart();   Log.d("CICLO", "onStart") }
override fun onResume()  { super.onResume();  Log.d("CICLO", "onResume") }
override fun onPause()   { super.onPause();   Log.d("CICLO", "onPause") }
override fun onStop()    { super.onStop();    Log.d("CICLO", "onStop") }
override fun onDestroy() { super.onDestroy(); Log.d("CICLO", "onDestroy") }
```

Abre **Logcat** (abajo en Android Studio), filtra por `CICLO` y prueba: ir al inicio,
volver, girar el móvil, cerrar la app. ¿Qué secuencia ves en cada caso?

> **💊 Píldora — `super`**
> `super.onStart()` significa "ejecuta primero lo que hacía la clase padre". Si lo quitas,
> Android se queja y la app se cierra: el padre tenía trabajo importante que hacer.

---

## Bloque 6 · Depurar sin perder la calma

- **Logcat**: `Log.d("ETIQUETA", "mensaje")` escribe mensajes que ves en Logcat. Es el
  `print` de Android.
- **Errores en rojo**: pon el ratón encima; muchas veces `Alt + Enter` propone la solución
  (por ejemplo, añadir un `import` que falta).
- **La app se cierra sola** (*crash*): en Logcat busca la primera línea roja con
  `FATAL EXCEPTION` y, debajo, la primera línea que mencione **tu** fichero.
- **Preview no se actualiza**: pulsa *Build & Refresh* en el panel de Preview.

### Errores típicos de principiante

| Síntoma | Causa probable |
|---------|----------------|
| `Unresolved reference: dp` / `Modifier` / … | Falta un `import`. `Alt + Enter`. |
| `@Composable invocations can only happen from the context of a @Composable function` | Llamas a `Text(...)` desde una función sin `@Composable`, o dentro de `onClick`. |
| El texto no cambia al pulsar | Usaste `var x = 0` en vez de `var x by remember { mutableStateOf(0) }`. |
| `Val cannot be reassigned` | Declaraste con `val` algo que luego cambias: usa `var`. |

---

## Proyecto · Reproductor de melodías por MQTT

Esta mini-app enlaza con las prácticas anteriores de la asignatura (sockets, sesión de IoT
con MQTT) y se construye en varios pasos:

| Paso | Qué hace |
|------|----------|
| [paso1.md](paso1.md) | Crear el proyecto: una Empty Activity con Compose que arranca y muestra "Hello Android!" |
| [paso2.md](paso2.md) | Preparar las herramientas: añadir las librerías de HTTP (OkHttp), JSON y MQTT (Eclipse Paho) |
| [paso3.md](paso3.md) | Usar las herramientas: descargar un JSON por HTTP, y publicarse/suscribirse a un topic MQTT |
| Paso 4 (lo hacéis vosotros) | La app final: descargar un JSON con un array de canciones y mostrarlo en pantalla como una lista. Al pulsar una canción se publica un mensaje MQTT que hace que un **ESP32-C3** reproduzca esa melodía. |

---

## Para seguir aprendiendo

- **Navegación entre pantallas**: *Navigation Compose*.
- **Guardar datos** entre sesiones: *DataStore* (preferencias) y *Room* (base de datos).
- **Arquitectura**: `ViewModel` para separar la lógica de la pantalla.

### Recursos
- Curso oficial gratuito: [Android Basics with Compose](https://developer.android.com/courses/android-basics-compose/course)
- Documentación de Compose: [developer.android.com/compose](https://developer.android.com/develop/ui/compose)
- Probar Kotlin en el navegador sin instalar nada: [play.kotlinlang.org](https://play.kotlinlang.org)
- Tour oficial de Kotlin: [kotlinlang.org/docs/kotlin-tour-welcome.html](https://kotlinlang.org/docs/kotlin-tour-welcome.html)
