# Introducción a Android en 2 Horas

## **Objetivos del Taller**
- Familiarizarse con **Android Studio** y su entorno.
- Comprender las bases de las **actividades** y su ciclo de vida.
- Introducción al diseño y a la interfaz de usuario en Android.

---

## **Sección 1: Introducción a Android Studio (20 minutos)**

### 1. ¿Qué es Android Studio?
- IDE oficial para el desarrollo de aplicaciones Android.
- Basado en IntelliJ IDEA.
- Herramientas principales: editor de código, emuladores, diseño de interfaz, herramientas de depuración, y más.

### 2. Configuración de Android Studio
- Instalación de Android Studio.
- Crear un nuevo proyecto:
  - Elección del tipo de actividad (Empty Activity, Basic Activity, etc.).
  - Configuración inicial (nombre del paquete, idioma, etc.).

### 3. Estructura de un Proyecto Android
- **`app/`**: Código fuente, recursos, y archivos de configuración.
- **`src/`**: Archivos Java o Kotlin.
- **`res/`**: Recursos como imágenes, layouts y cadenas de texto.
- **`AndroidManifest.xml`**: Configuración de la aplicación y sus componentes.

---

## **Sección 2: Actividades en Android (30 minutos)**

### 1. ¿Qué es una Actividad?
- La unidad básica de una aplicación Android.
- Cada pantalla de la aplicación generalmente corresponde a una actividad.

### 2. Creación y estructura de una Actividad
- Clase `Activity`:
  - Herencia de `AppCompatActivity` (para compatibilidad con versiones antiguas).
  - Método `onCreate()` y `setContentView()`.

### 3. Ciclo de Vida de una Actividad (Fundamental)
- **Métodos principales**:
  - `onCreate()`: Se llama cuando la actividad es creada.
  - `onStart()`: La actividad es visible, pero no interactiva.
  - `onResume()`: La actividad está interactiva y visible.
  - `onPause()`: La actividad aún está visible pero no interactiva.
  - `onStop()`: La actividad ya no es visible.
  - `onDestroy()`: La actividad es destruida.
- **Flujo del ciclo de vida**: Como Android maneja las actividades en segundo plano y su reactivación.

### 4. Ejemplo práctico: Crear una actividad
- Crear una actividad simple en Android Studio.
- Mostrar un saludo en la pantalla (con un `TextView`).
- Incluir el ciclo de vida en los logs (usando `Log.d()`).

---

## **Sección 3: Manejo de la Interfaz de Usuario (30 minutos)**

### 1. Layouts en Android
- **`LinearLayout`**: Organiza los elementos en fila o columna. Simple.
- **`RelativeLayout`**: Organiza elementos en relación entre sí. En desuso.
- **`ConstraintLayout`**: Layout flexible y eficiente (recomendado).
- **`FrameLayout`**: Un solo contenedor para otros elementos.

### 2. Componentes de la Interfaz de Usuario (UI)
- **Botones** (`Button`), **Textos** (`TextView`), **Campos de Entrada** (`EditText`), **Imágenes** (`ImageView`), **Listas** (`RecyclerView`).
- Uso de **`XML`** para definir el diseño.
- Práctica: Crear un layout con un `Button` y un `TextView`.

### 3. Estilos y Temas
- ¿Qué son los temas en Android? 
  - Definir colores, tipografías, estilos globales.
  - Cómo cambiar el tema de la aplicación en el archivo `res/values/styles.xml`.
- **Material Design**: Guía de diseño de Google.
  - Elementos básicos como botones flotantes (FAB), tarjetas, animaciones.

---

## **Sección 4: Manejo de Eventos y Navegación (30 minutos)**

### 1. Manejo de Eventos de Usuario
- **Listeners**: Detectar interacciones con botones y otros componentes de UI.
  - Ejemplo: `Button.setOnClickListener()`.
- **Ejemplo práctico**: Crear un botón que cambie el texto de un `TextView` al hacer clic.

### 2. Navegación entre Actividades
- **Intents**: ¿Cómo iniciar una nueva actividad?
  - Crear un `Intent` explícito o implícito.
  - **Intent explícito**: `Intent(this, NewActivity::class.java)`.
  - **Intent implícito**: Usado para acciones generales (por ejemplo, abrir una URL).
- **Pasar datos entre actividades**:
  - **PutExtra()**: Enviar datos con el `Intent`.
  - **GetExtras()**: Obtener datos al recibir el `Intent` en la nueva actividad.
- **Ejemplo práctico**: Crear dos actividades y pasar datos entre ellas.

---

## **Sección 5: Fundamentos de Persistencia de Datos (20 minutos)**

### 1. ¿Por qué es importante la persistencia?
- Mantener datos entre sesiones de la aplicación.
- Guardar información como preferencias del usuario, configuraciones, etc.

### 2. Tipos de almacenamiento:
- **SharedPreferences**: Almacenamiento clave-valor para configuraciones simples.
- **Bases de datos SQLite**: Almacenar datos estructurados.
- **Archivos**: Leer y escribir archivos locales.

### 3. Ejemplo práctico: Usar SharedPreferences
- Guardar y leer configuraciones simples (por ejemplo, preferencia de idioma).

---

## **Sección 6: Depuración y Pruebas (10 minutos)**

### 1. Herramientas de Depuración en Android Studio
- Uso del Logcat para ver mensajes de log.
- Puntos de interrupción en el código (debugging).
- Analizar el ciclo de vida con logs.

### 2. Pruebas en Android
- **Emuladores**: Cómo usar emuladores de Android Studio para probar aplicaciones.
- **Dispositivos Físicos**: Cómo conectar tu dispositivo Android para probar la app.
- **Pruebas Unitarias y UI**: Introducción a las pruebas en Android (JUnit, Espresso).

---

## **Conclusión (10 minutos)**

### 1. Resumen
- Revisión de los temas cubiertos: actividades, ciclo de vida, UI, navegación, persistencia de datos, etc.
- ¿Qué sigue? Continuar con proyectos prácticos para dominar Android.

### 2. Recursos adicionales
- Documentación oficial de Android: [developer.android.com](https://developer.android.com/)
- Tutoriales y cursos online recomendados.

### 3. Preguntas y respuestas
- Espacio para aclarar dudas y responder preguntas de los participantes.
