# Mini-tutorial de Kotlin

> Kotlin es el lenguaje oficial para Android. Es moderno, conciso y te protege de muchos
> errores. **Todo lo de este documento puedes probarlo en el navegador**, sin instalar
> nada, en [play.kotlinlang.org](https://play.kotlinlang.org).

Volver al [README](readme.MD) · Siguiente: [compose.md](compose.md)

---

## 1. El programa más pequeño

```kotlin
fun main() {
    println("¡Hola, mundo!")
}
```

- `fun` declara una **función** (un bloque de instrucciones con nombre).
- `main` es la función por la que empieza el programa (en Android no la usarás: allí manda `onCreate`).
- `println` escribe un texto y salta de línea.
- No hacen falta `;` al final de cada línea.

---

## 2. Variables: `val` y `var`

```kotlin
val nombre = "Lucía"     // val = valor fijo (no se puede cambiar)
var edad = 20            // var = variable (sí se puede cambiar)
edad = 21                // ✅
// nombre = "Ana"        // ❌ error: Val cannot be reassigned
```

**Consejo**: usa `val` siempre que puedas y `var` solo cuando el dato tenga que cambiar.

### Tipos básicos

| Tipo | Ejemplo | Para qué |
|------|---------|----------|
| `Int` | `42` | números enteros |
| `Double` | `3.14` | números con decimales |
| `String` | `"hola"` | texto |
| `Boolean` | `true`, `false` | sí / no |

Kotlin **adivina** el tipo (`val x = 5` → `Int`), pero puedes escribirlo:
`val x: Int = 5`.

### Plantillas de texto
```kotlin
val precio = 3.5
println("Cuesta $precio euros")              // Cuesta 3.5 euros
println("El doble son ${precio * 2} euros")  // expresiones entre ${ }
```

---

## 3. Decisiones: `if` y `when`

```kotlin
val nota = 7.2

if (nota >= 5) {
    println("Aprobado")
} else {
    println("Suspenso")
}

// if también puede DEVOLVER un valor
val resultado = if (nota >= 5) "Apto" else "No apto"

// when = varios casos (como un "switch" mejorado)
val calificacion = when {
    nota >= 9 -> "Sobresaliente"
    nota >= 7 -> "Notable"
    nota >= 5 -> "Aprobado"
    else      -> "Suspenso"
}
```

---

## 4. Repeticiones: `for` y `while`

```kotlin
for (i in 1..5) {          // 1, 2, 3, 4, 5
    println("Vuelta $i")
}

val frutas = listOf("manzana", "pera", "kiwi")
for (fruta in frutas) {
    println(fruta)
}

var n = 3
while (n > 0) {
    println(n)
    n--                    // n = n - 1
}
```

---

## 5. Funciones

```kotlin
fun saludar(nombre: String): String {
    return "Hola, $nombre"
}

// Si la función es de una sola expresión, se puede abreviar:
fun cuadrado(x: Int) = x * x

// Parámetros con valor por defecto
fun presentar(nombre: String, saludo: String = "Hola") = "$saludo, $nombre"

presentar("Pablo")                    // "Hola, Pablo"
presentar("Pablo", saludo = "Buenas") // "Buenas, Pablo"  ← parámetro con nombre
```

> Los **parámetros con nombre** y **por defecto** se usan muchísimo en Compose:
> `Text(text = "Hola", fontSize = 20.sp)`.

---

## 6. Lambdas

Una **lambda** es una función sin nombre, escrita entre llaves, que se puede guardar en
una variable o pasar a otra función.

```kotlin
val doble = { x: Int -> x * 2 }
println(doble(4))   // 8

val numeros = listOf(1, 2, 3, 4, 5)
val pares = numeros.filter { it % 2 == 0 }   // [2, 4]   "it" = cada elemento
val dobles = numeros.map { it * 2 }          // [2, 4, 6, 8, 10]
```

**Truco de sintaxis**: si el último parámetro de una función es una lambda, se puede
escribir **fuera** de los paréntesis:

```kotlin
boton.alPulsar({ println("pulsado") })   // forma "larga"
boton.alPulsar { println("pulsado") }    // forma habitual
```

Por eso en Compose ves cosas como `Column { … }` o `Button(onClick = { … }) { Text("OK") }`:
¡son funciones que reciben lambdas!

---

## 7. Null safety (el signo `?`)

`null` significa "no hay nada". En muchos lenguajes usar algo que es `null` hace que el
programa explote. Kotlin te obliga a decir **de antemano** qué puede ser `null`.

```kotlin
var a: String = "hola"
// a = null                   // ❌ no compila

var b: String? = "hola"       // el ? dice: "puede ser null"
b = null                      // ✅

println(b?.length)            // ?.  → si b es null, da null en vez de explotar
println(b?.length ?: 0)       // ?:  → "si es null, usa 0" (operador Elvis)

val numero = "12a".toIntOrNull()   // null, porque "12a" no es un número
```

---

## 8. Listas

```kotlin
val fija = listOf("a", "b", "c")            // no se puede modificar
val cambiante = mutableListOf("a", "b")     // sí se puede
cambiante.add("c")
cambiante.remove("a")

println(fija[0])        // "a"   (se empieza a contar en 0)
println(fija.size)      // 3
```

---

## 9. Clases y objetos

Una **clase** es un **molde**; un **objeto** es una cosa concreta hecha con ese molde.

```kotlin
class Coche(val marca: String, var velocidad: Int = 0) {
    fun acelerar(cuanto: Int) {
        velocidad += cuanto
    }
}

val miCoche = Coche("Seat")      // creamos un objeto (no hace falta "new")
miCoche.acelerar(50)
println(miCoche.velocidad)       // 50
```

### `data class`: clases para guardar datos
```kotlin
data class Alumno(val nombre: String, val nota: Double)

val a = Alumno("Marta", 8.5)
println(a)                       // Alumno(nombre=Marta, nota=8.5)  ← se imprime bonito
val b = a.copy(nota = 9.0)       // copia cambiando solo un campo
```

---

## 10. Herencia

La herencia permite crear una clase nueva **basada en otra**. La nueva (hija) tiene todo
lo de la original (padre) y puede **añadir** o **cambiar** cosas.

```kotlin
open class Animal(val nombre: String) {       // open: permite heredar
    open fun hablar(): String = "…"           // open: permite sobrescribir
    fun presentarse() = "Soy $nombre y digo ${hablar()}"
}

class Perro(nombre: String) : Animal(nombre) {
    override fun hablar() = "¡Guau!"
}

class Gato(nombre: String) : Animal(nombre) {
    override fun hablar() = "¡Miau!"
}

val animales = listOf(Perro("Toby"), Gato("Misi"))
for (an in animales) println(an.presentarse())
// Soy Toby y digo ¡Guau!
// Soy Misi y digo ¡Miau!
```

Ideas clave:
- `: Animal(nombre)` → "Perro **es un** Animal".
- En Kotlin las clases son **cerradas** por defecto: hay que marcarlas `open` para heredar.
- `override` → "reemplazo la versión del padre por la mía".
- `super.metodo()` → "llama también a la versión del padre".

**En Android**:
```kotlin
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)   // primero, lo que hacía el padre
        // … luego, lo nuestro
    }
}
```

### Interfaces (un "contrato")
Una **interfaz** dice *qué* debe saber hacer una clase, sin decir *cómo*.

```kotlin
interface Volador {
    fun volar(): String
}

class Pato(nombre: String) : Animal(nombre), Volador {
    override fun hablar() = "¡Cuac!"
    override fun volar() = "$nombre vuela bajito"
}
```

Una clase solo puede heredar de **una** clase, pero puede cumplir **varias** interfaces.

---

## 11. Chuleta rápida

| Quiero… | Kotlin |
|---------|--------|
| Dato fijo | `val x = 1` |
| Dato que cambia | `var x = 1` |
| Puede estar vacío | `var x: String? = null` |
| Si es null, usar otro | `x ?: "por defecto"` |
| Función | `fun f(a: Int): Int { return a + 1 }` |
| Función corta | `fun f(a: Int) = a + 1` |
| Lambda | `{ a: Int -> a + 1 }` |
| Lista | `listOf(1, 2, 3)` / `mutableListOf()` |
| Clase | `class P(val nombre: String)` |
| Heredar | `class Hija : Padre()` (Padre debe ser `open`) |
| Sobrescribir | `override fun …` |

---

## Ejercicios (en play.kotlinlang.org)

1. Escribe una función `esPar(n: Int): Boolean`.
2. Dada `listOf(3, 8, 1, 9, 4)`, imprime solo los mayores que 4 usando `filter`.
3. Crea una `data class Producto(val nombre: String, val precio: Double)` y una lista de
   productos. Calcula el precio total con `sumOf { it.precio }`.
4. Crea una clase `open class Figura` con `open fun area(): Double` y dos hijas,
   `Circulo(r)` y `Rectangulo(b, h)`, que sobrescriban `area()`. Recorre una lista de
   figuras e imprime sus áreas.
