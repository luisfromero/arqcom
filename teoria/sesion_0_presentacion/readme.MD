# Arquitectura de Computadores
## Ingeniería en Tecnologías Industriales

> Curso 2026/2027

![Arquitectura moderna](https://images.unsplash.com/photo-1518770660439-4636190af475?auto=format&fit=crop&w=1200&q=80)

© Luis Felipe Romero Gómez

### Guía docente

La asignatura estudia la organización interna de los computadores modernos, con especial atención a la comprensión de los conceptos generales que permiten entender cómo funcionan los procesadores actuales, los sistemas empotrados, los dispositivos móviles y las arquitecturas orientadas a Internet de las Cosas (IoT).

- [Guía docente (PDF)](./Guia_Programa.pdf)
- [Campus Virtual](https://eii.cv.uma.es/course/view.php?id=193)
- [Repositorio de la asignatura](https://github.com/luisfromero/arqcom)

---

## 1. ¿Qué es esta asignatura?

Arquitectura de Computadores es una asignatura clave dentro del grado de Ingeniería en Tecnologías Industriales porque conecta la teoría del hardware con la práctica del software y de los sistemas reales.

No se trata solo de estudiar cómo funciona una CPU, sino de entender:

- por qué un programa tarda más o menos en ejecutarse,
- cómo se organiza la memoria y por qué importa la jerarquía de caché,
- qué es un procesador segmentado y superescalar,
- cómo se diseñan sistemas con múltiples núcleos,
- qué papel juegan los microcontroladores y las arquitecturas IoT,
- y cómo la programación en bajo nivel y el software aplicado influyen en el rendimiento real del sistema.

La asignatura mantiene la base formal de la guía docente, pero se orienta hacia un enfoque más actual, práctico y atractivo para estudiantes de GITI, conectando la arquitectura con la programación, los dispositivos físicos y la ingeniería aplicada.

---

## 2. Objetivos principales

Al finalizar el curso, el estudiante debe ser capaz de:

- comprender la organización interna de un computador moderno,
- analizar el rendimiento de una arquitectura y sus limitaciones,
- entender la estructura básica de un procesador segmentado,
- conocer el papel de la memoria y de la jerarquía de caché,
- comprender el paralelismo a nivel de instrucción y la evolución hacia arquitecturas multicore,
- interpretar programas en ensamblador y relacionarlos con la arquitectura del procesador,
- reconocer los principios básicos de microcontroladores, sistemas embebidos e IoT,
- relacionar la arquitectura con la programación y con la ingeniería aplicada a sistemas reales.

---

## 3. Enfoque moderno de la asignatura

La guía oficial sigue manteniendo la base tradicional de la arquitectura de computadores, pero en este curso se prioriza un enfoque más cercano a la ingeniería actual:

- Procesadores modernos y rendimiento real
- Multicore, paralelismo y optimización software/hardware
- Microcontroladores y sistemas embebidos
- Internet de las cosas (IoT) y comunicaciones
- Desarrollo práctico con C, Python, JavaScript, Android y más
- Integración de la programación con la arquitectura del sistema

> Se conserva la parte esencial de la arquitectura del procesador y de la memoria, pero se evita una profundización excesiva en temas más históricos o muy específicos que no aportan tanto valor para la formación actual del estudiante.

---

## 4. Contenidos del curso

### 4.1. Arquitectura del procesador

- Introducción a la arquitectura de computadores
- Rendimiento de los computadores
- Procesadores segmentados
- Soluciones básicas a los problemas de segmentación
- Anticipación y mecanismos hardware
- Segmentación avanzada
- Ejecutión fuera de orden
- Predicción de saltos
- Procesadores superescalares
- Paralelismo a nivel de instrucción y de máquina

### 4.2. Arquitectura de la memoria

- Memoria del computador
- Jerarquía de memoria y principio de localidad
- Conceptos básicos de caché
- Relación entre CPU, caché y memoria principal
- Buses y conexión entre componentes

### 4.3. Paralelismo y arquitecturas multicore

- Taxonomía del paralelismo
- Arquitecturas paralelas
- Paralelismo espacial y temporal
- Medidas de rendimiento
- Consistencia de memoria y coherencia de caché

### 4.4. Microcontroladores, IoT y sistemas embebidos

- Introducción a microcontroladores
- Arquitecturas RISC-V y sistemas embebidos
- Sensores, actuadores y periféricos
- Comunicaciones: WiFi, Bluetooth, BLE, LoRa, MQTT
- IoT aplicado a la industria y a la automatización
- Plataformas y prototipado con placas de desarrollo

![IoT y microcontroladores](https://images.unsplash.com/photo-1518770660439-4636190af475?auto=format&fit=crop&w=1200&q=80)

### 4.5. Programación y tecnologías actuales

- C y C++ para sistemas de bajo nivel
- Python para análisis y automatización
- JavaScript y desarrollo web
- PHP y aplicaciones web
- Android y desarrollo móvil
- Git, VS Code y Copilot como herramientas de trabajo
- MQTT y n8n para integración de sistemas

---

## 5. Herramientas y tecnologías del curso

La asignatura combina teoría con herramientas reales de ingeniería:

- Lenguajes: C, C++, C#, Python, JavaScript, Kotlin
- Desarrollo: Git, VS Code, GitHub Copilot
- IoT: MQTT, n8n, dispositivos conectados
- Móviles: Android
- Web: HTML, CSS, JavaScript, PHP
- Microcontroladores: arquitecturas RISC-V y placas de desarrollo
- Comunicaciones: WiFi, Bluetooth, BLE, LoRa

![Herramientas de ingeniería](https://images.unsplash.com/photo-1516321318423-f06f85e504b3?auto=format&fit=crop&w=1200&q=80)

---

## 6. Calendario y planificación

La planificación del curso se estructura alrededor de los grandes bloques del programa:

1. Introducción a la arquitectura y rendimiento
2. Procesadores segmentados y superescalares
3. Memoria y caché
4. Paralelismo y multicore
5. Microcontroladores y dispositivos embebidos
6. IoT, comunicaciones y sistemas conectados
7. Programación aplicada y proyectos prácticos

> En la práctica, el calendario puede adaptarse al ritmo de la clase, pero la secuencia de contenidos se mantiene fiel a la guía docente.

---

## 7. Bibliografía

### Básica

- Hennessy, J. L. y Patterson, D. A. — Arquitectura de Computadores: Un Enfoque Cuantitativo
- Hennessy, J. L. y Patterson, D. A. — Computer Organization and Design: The Hardware/Software Interface

### Complementaria

- Hwang, K. y Briggs, F. — Arquitectura de Computadores y Procesamiento Paralelo
- Tanenbaum, A. S. — Structured Computer Organization

---

## 8. Evaluación

La guía docente establece que la evaluación se realiza principalmente a través de pruebas escritas y actividades relacionadas con los contenidos impartidos.

### Primera convocatoria

- Examen escrito sobre los contenidos teóricos impartidos en el periodo lectivo anterior.
- El examen se divide en dos partes, que deben superarse por separado.
- La nota final se obtiene como media de las calificaciones obtenidas en cada parte.
- La asistencia a clase es obligatoria y cualquier ausencia debe justificarse adecuadamente.

### Segunda convocatoria y extraordinaria

- Se realiza un examen único que cubre toda la asignatura.

### Recomendaciones

- La asignatura requiere trabajo continuado y comprensión de la relación entre hardware, software y rendimiento.
- La práctica y la resolución de problemas son esenciales para comprender los conceptos.

---

## 9. Competencias y resultados esperados

Para superar la asignatura, el alumno debe:

- conocer en detalle la arquitectura del juego de instrucciones de un computador,
- codificar y decodificar programas en lenguaje ensamblador,
- interpretar trozos de código ensamblador y máquina,
- comprender la estructura de un procesador segmentado y superescalar,
- conocer la arquitectura del sistema jerárquico de memoria,
- entender los mecanismos del multicore y del paralelismo,
- relacionar estas ideas con sistemas embebidos, IoT y programación aplicada.

---

## 10. FAQ

### ¿En qué consiste el examen final?

El examen final consiste en una prueba escrita que evalúa la comprensión de los contenidos teóricos y conceptuales de la asignatura, con ejercicios y preguntas que requieren razonamiento sobre arquitectura, rendimiento y diseño de sistemas.

### ¿Dónde encuentro más información?

- Campus Virtual: https://eii.cv.uma.es/course/view.php?id=193
- Repositorio de la asignatura: https://github.com/luisfromero/arqcom

### Contacto

- Email: felipe@uma.es
- Teléfono: 952134169
- Despacho: 2.2.47 en la ETSI Informática
- También disponible por WhatsApp: 656358240

---

## 11. Mensaje final

Arquitectura de Computadores no es solo una asignatura de teoría de hardware: es una asignatura que conecta lo que hay «dentro» del ordenador con lo que el estudiante puede construir, programar y entender en el mundo real.

La idea es combinar rigor académico con visión aplicada, mostrando que la arquitectura de computadores no termina en el procesador, sino que se extiende a los sistemas embebidos, a la programación, a las comunicaciones, a la automatización y a la inteligencia ubicua.

Si aprendes esta asignatura bien, no solo entiendes cómo funciona un ordenador: entiendes cómo se diseña el mundo digital que nos rodea.

---

## 12. Enlaces útiles

- [Campus Virtual](https://eii.cv.uma.es/course/view.php?id=193)
- [Repositorio GitHub](https://github.com/luisfromero/arqcom)
- [Visual Studio Code](https://code.visualstudio.com/)
- [GitHub Copilot](https://github.com/features/copilot)
- [GitHub Education](https://education.github.com/)


