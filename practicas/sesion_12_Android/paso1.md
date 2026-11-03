# Paso 1 – Crear el proyecto (Empty Activity con Compose)

En este paso creamos una app vacía con Android Studio y comprobamos que se
ejecuta. Al terminar veremos en pantalla el texto **"Hello Android!"**.

---

## 1. Crear un proyecto nuevo

1. Abre **Android Studio**.
2. Pulsa **New Project**. Si ya tienes un proyecto abierto, usa el menú
   **File → New → New Project...**
3. En la lista de plantillas (*Phone and Tablet*), elige **Empty Activity**.

   > Fíjate en que sea **Empty Activity**, la que tiene el logotipo de Compose.
   > No elijas *Empty Views Activity*: esa usa el sistema antiguo de pantallas en XML.

4. Pulsa **Next**.

---

## 2. Configurar el proyecto

Rellena el formulario así:

| Campo | Valor | Explicación |
|---|---|---|
| **Name** | `My Application` | Nombre de la app que verá el usuario |
| **Package name** | `com.helioserver.myapplication` | Identificador único de la app. Puedes usar el tuyo, pero los ejemplos de los pasos siguientes usan este |
| **Save location** | `D:\dev\proyectos\app1` | Carpeta donde se guarda el proyecto |
| **Minimum SDK** | `API 30 ("R"; Android 11.0)` | Versión de Android más antigua en la que funcionará la app |
| **Build configuration language** | `Kotlin DSL (build.gradle.kts)` | Es la opción recomendada |

Pulsa **Finish**.

---

## 3. Esperar a que termine la preparación

La primera vez Android Studio descarga muchas cosas y puede tardar varios minutos.

- Abajo verás una barra de progreso con mensajes como *Gradle sync*, *Indexing*...
- **Espera a que desaparezca** antes de tocar nada.
- Cuando termine, no debe haber errores en rojo en la pestaña **Build**.

---

## 4. Conocer los ficheros importantes

En el panel *Project* (a la izquierda), cambia la vista de **Android** a **Project**
si quieres ver las carpetas tal y como están en el disco.

```
app1/
├── gradle/
│   └── libs.versions.toml          ← versiones de las librerías
├── app/
│   ├── build.gradle.kts            ← configuración y librerías de la app
│   └── src/main/
│       ├── AndroidManifest.xml     ← permisos y pantallas de la app
│       └── java/com/helioserver/myapplication/
│           ├── MainActivity.kt     ← la pantalla principal
│           └── ui/theme/           ← colores y estilos
└── settings.gradle.kts
```

Abre `MainActivity.kt`. Contiene algo parecido a esto:

```kotlin
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            MyApplicationTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    Greeting(
                        name = "Android",
                        modifier = Modifier.padding(innerPadding)
                    )
                }
            }
        }
    }
}

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Hello $name!",
        modifier = modifier
    )
}
```

- `MainActivity` es la pantalla que se abre al arrancar la app.
- `setContent { ... }` define lo que se dibuja, usando **Jetpack Compose**.
- `Greeting` es una función `@Composable`, es decir, un trozo de interfaz. Aquí
  solo muestra un texto.

---

## 5. Preparar un dispositivo donde ejecutar la app

Tienes dos opciones:

**Opción A: emulador (móvil virtual)**

1. Abre **Device Manager**, con el icono del móvil en la barra lateral derecha
   o desde **View → Tool Windows → Device Manager**.
2. Pulsa **+ → Create Virtual Device**.
3. Elige un móvil (por ejemplo *Pixel 8*) → **Next**.
4. Elige una imagen del sistema reciente. Si aparece un icono de descarga ⬇,
   descárgala → **Next** → **Finish**.

**Opción B: móvil real**

1. En el móvil, ve a **Ajustes → Información del teléfono** y pulsa 7 veces
   sobre **Número de compilación**. Esto activa las *Opciones de desarrollador*.
2. En **Ajustes → Sistema → Opciones de desarrollador**, activa
   **Depuración por USB**.
3. Conecta el móvil al ordenador con un cable USB y acepta el aviso que aparece
   en el móvil.

---

## 6. Ejecutar la app

1. Arriba, en la barra de herramientas, selecciona tu dispositivo en el desplegable.
2. Pulsa el botón verde ▶ **Run 'app'** (o `Mayús + F10`).
3. Espera a que compile e instale.

Deberías ver en pantalla:

```
Hello Android!
```

¡Listo! Ya tienes la base del proyecto.

---

## Problemas frecuentes

| Problema | Solución |
|---|---|
| El desplegable de dispositivos dice *No devices* | Crea un emulador (opción A) o revisa el cable y la depuración USB (opción B) |
| Errores en rojo justo después de crear el proyecto | Espera a que termine *Gradle sync*. Si siguen, pulsa 🐘 **Sync Project with Gradle Files** |
| El emulador va muy lento | Cierra otros programas. La primera vez siempre tarda más en arrancar |

---

## Siguiente paso

En el **paso 2** añadiremos las herramientas para usar HTTP, JSON y MQTT
(ver `paso2.md`).
