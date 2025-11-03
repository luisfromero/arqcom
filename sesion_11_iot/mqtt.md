# Introducción a IoT y MQTT (Lectura de 10 minutos)

Bienvenido al fascinante mundo del **Internet de las Cosas (IoT)**. En esta guía rápida, entenderás qué es IoT, por qué necesita un lenguaje especial para comunicarse y cómo el protocolo **MQTT** se ha convertido en el estándar de facto para ello.

---

## 1. ¿Qué es el Internet de las Cosas (IoT)?

Imagina conectar objetos cotidianos a internet: tu reloj, las luces de casa, el termostato, un sensor de humedad en una maceta o incluso el collar de tu mascota. Cuando estos objetos pueden enviar datos (como la temperatura actual) o recibir órdenes (como "enciéndete"), se convierten en parte del "Internet de las Cosas".

En resumen, **IoT es la red de objetos físicos ("cosas") que llevan incorporados sensores, software y otras tecnologías con el fin de conectarse e intercambiar datos con otros dispositivos y sistemas a través de internet.**

El objetivo es simple: hacer que los objetos "tontos" se vuelvan "inteligentes" para:
-   **Recopilar datos** del mundo real (temperatura, movimiento, consumo eléctrico).
-   **Automatizar acciones** (si la humedad baja, activa el riego; si anochece, enciende las luces).
-   **Tomar decisiones informadas** (analizar el consumo para ahorrar energía).

### El Reto: ¿Cómo se comunican millones de dispositivos?

A diferencia de un navegador web que pide una página a un servidor (usando HTTP), los dispositivos IoT tienen necesidades muy diferentes:

-   **Bajo consumo**: Muchos funcionan con baterías y deben durar meses o años.
-   **Redes inestables**: La conexión Wi-Fi o móvil puede ser débil o intermitente.
-   **Bajo ancho de banda**: No pueden permitirse enviar grandes cantidades de datos.
-   **Comunicación bidireccional**: Un dispositivo necesita tanto enviar datos como recibir órdenes.

Usar HTTP para cada pequeño dato sería como enviar una carta certificada para decir una sola palabra: ineficiente y caro en términos de energía y datos. Aquí es donde entra MQTT.

---

## 2. MQTT: El Lenguaje de los Dispositivos IoT

**MQTT (Message Queuing Telemetry Transport)** es un protocolo de mensajería diseñado específicamente para los desafíos de IoT. Es extremadamente ligero, eficiente y fiable, incluso en redes de mala calidad.

Su magia reside en un modelo llamado **Publicar/Suscribir (Pub/Sub)**.

### El Modelo Publicar/Suscribir

En lugar de que los dispositivos se conecten directamente entre sí, todo pasa a través de un intermediario central llamado **Broker**.

Los tres componentes clave son:

1.  **Publisher (Publicador)**: Un dispositivo que envía un mensaje. Por ejemplo, un sensor de temperatura que "publica" el valor `25.5`.
2.  **Subscriber (Suscriptor)**: Un dispositivo o aplicación que quiere recibir mensajes. Por ejemplo, una app en tu móvil que se "suscribe" para recibir los valores de temperatura.
3.  **Broker (Intermediario)**: El servidor central. Recibe todos los mensajes de los publicadores y los reenvía a los suscriptores interesados.

#### La Analogía del Periódico

Imagina que el Broker es un **quiosco de periódicos**.
-   Los **publicadores** son los periodistas que escriben artículos sobre temas específicos (deportes, política, tecnología).
-   Los **suscriptores** son los lectores que le dicen al quiosquero: "Solo me interesan los artículos de tecnología".
-   El quiosquero (el **Broker**) recibe todos los artículos y solo entrega los de tecnología a los lectores que lo pidieron.

El periodista no sabe quién leerá su artículo, y el lector no sabe quién lo escribió. Están **desacoplados**. Solo necesitan conocer al quiosquero y el nombre de la sección.

### Temas (Topics): Las "Secciones del Periódico"

En MQTT, las "secciones" se llaman **temas (topics)**. Un tema es simplemente una cadena de texto con una estructura jerárquica, similar a las carpetas de un sistema de archivos.

Por ejemplo:

-   `casa/salon/temperatura`: Un sensor en el salón publica la temperatura aquí.
-   `casa/salon/luz/estado`: El estado de la luz del salón (`on` u `off`).
-   `casa/salon/luz/comando`: Para enviar órdenes a la luz (`on` o `off`).

Un publicador envía un mensaje a un tema específico. Un suscriptor le dice al Broker a qué temas quiere suscribirse.

**Ejemplo práctico:**

1.  Un **sensor de temperatura** (Publisher) publica el mensaje `22.5` en el tema `casa/dormitorio/temperatura`.
2.  El **Broker** recibe este mensaje.
3.  Una **app de domótica** (Subscriber), que previamente se suscribió al tema `casa/dormitorio/temperatura`, recibe el mensaje `22.5` del Broker y actualiza su interfaz.



### ¿Por qué es tan bueno para IoT?

-   **Eficiencia**: Los mensajes son muy pequeños. El encabezado de un mensaje MQTT puede ser de solo 2 bytes, frente a los cientos de bytes de una petición HTTP.
-   **Desacoplamiento**: Los dispositivos no necesitan conocerse entre sí. Puedes añadir o quitar sensores y aplicaciones sin tener que reprogramar todo el sistema. Solo necesitan hablar con el Broker.
-   **Fiabilidad**: MQTT tiene mecanismos (niveles de Calidad de Servicio o QoS) para asegurar que un mensaje se entregue, incluso si la red falla temporalmente.
-   **Escalabilidad**: Un solo Broker puede gestionar miles de dispositivos conectados simultáneamente.

---

## 3. MQTT en Nuestra Práctica

En esta sesión, estamos usando varios componentes que encajan perfectamente en este modelo:

-   **Mosquitto**: Es nuestro **Broker** MQTT. Es el quiosco central que gestiona toda la comunicación.
-   **Ecowitt / `ecowitt2mqtt`**: La estación meteorológica es un **Publisher**. Envía datos de sensores (lluvia, viento, etc.) a temas como `ecowitt/station/outdoortemperature`.
-   **Shelly**: Es tanto un **Publisher** (publica su estado, `on`/`off`) como un **Subscriber** (se suscribe a un tema de comandos para recibir órdenes).
-   **n8n**: Actúa principalmente como **Subscriber** (escucha los datos de la estación Ecowitt) y también como **Publisher** (publica comandos para controlar el Shelly).
-   **MQTT Explorer**: Es una herramienta universal que puede ser tanto **Publisher** como **Subscriber**. Nos permite ver todos los mensajes que pasan por el Broker y enviar nuestros propios mensajes para probar cosas.

## Conclusión

IoT no es solo conectar cosas a internet; es hacer que se comuniquen de manera eficiente y escalable. MQTT, con su modelo Pub/Sub y su diseño ligero, proporciona la columna vertebral perfecta para esta comunicación, permitiendo que un ecosistema de dispositivos diversos colabore de forma flexible y fiable.

Ahora que entiendes la teoría, ¡estás listo para verla en acción!

