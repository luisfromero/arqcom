Quiero añadir a mi app Android (Kotlin + Jetpack Compose + Material 3) una pantalla de **ofertas de vuelos** que consuma una API REST propia. Genera el código completo y explica brevemente cada archivo. Usa ViewModel + StateFlow, corrutinas, Retrofit 2 + OkHttp + Gson. Nada de llamadas de red en el hilo principal.

## 1. Servidor

- URL base actual: `http://150.214.109.26:8000/` (HTTP sin TLS, en otra máquina de la red de la universidad).
- URL base futura: `https://casium.uma.es/vuelos-api/` (HTTPS, accesible desde fuera).
- Documentación OpenAPI/Swagger: `http://150.214.109.26:8000/docs` (esquema en `/openapi.json`).
- Prueba de conectividad sin autenticación: `GET health` → `{"status":"ok","tracking":true,"auth":true}`.

Requisitos de configuración:

1. La URL base debe estar en **un único sitio**: un `buildConfigField` `API_BASE_URL` en `app/build.gradle.kts` (activa `buildFeatures { buildConfig = true }`), para cambiar de una URL a otra tocando solo esa línea.
2. **La URL base termina en `/` y las rutas de Retrofit NO empiezan por `/`** (`@GET("api/deals/latest")`, no `@GET("/api/deals/latest")`). Es imprescindible: con la URL futura `.../vuelos-api/`, una ruta con `/` inicial descartaría el `vuelos-api` del path.
3. Autenticación: todas las rutas `api/...` exigen la cabecera `X-API-Key`. Lee la clave de `local.properties` (`FLIGHTS_API_KEY=...`) y exponla como `BuildConfig.API_KEY`. Añádela con un interceptor de OkHttp.
4. Como la URL actual es `http://`, añade `res/xml/network_security_config.xml` que permita tráfico en claro **solo** para el dominio `150.214.109.26`, y referéncialo en el `AndroidManifest.xml` (`android:networkSecurityConfig`). Añade un comentario indicando que se elimina al pasar a HTTPS. Incluye el permiso `INTERNET`.
5. Timeouts de OkHttp: conexión 15 s, lectura 90 s. Añade `HttpLoggingInterceptor` (nivel BODY) solo en debug.

## 2. Endpoints a usar

Todas las fechas en formato `YYYY-MM-DD`. Días de la semana: `0`=lunes … `6`=domingo. Precios en EUR, ida y vuelta por persona. Origen fijo: Málaga (`AGP`).

### A) `GET api/deals/latest` — ofertas precalculadas (respuesta instantánea, usar SIEMPRE primero)

Query params (todos opcionales): `max_price` (Double, defecto 100), `weekdays` (String, ej. `"4"` o `"3,4,5"`), `min_nights` (Int), `max_nights` (Int), `date_from`, `date_to` (String), `destinations` (String, ej. `"LON,ROM"`), `max_stops` (Int), `limit` (Int, defecto 100).

Respuesta:
```json
{
  "source": "precomputed",
  "updated_at": "2026-11-18T11:55:20+01:00",
  "age_minutes": 12,
  "scan_running": null,
  "coverage": {"origin": "AGP", "date_from": "2026-11-20", "date_to": "2027-01-14",
               "weekdays": [3, 4, 5], "min_nights": 2, "max_nights": 4},
  "coverage_ok": true,
  "coverage_notes": [],
  "deals_count": 39,
  "deals": [
    {
      "destination": "LIS", "city": "Lisboa", "country": "Portugal",
      "departure_date": "2026-12-11", "return_date": "2026-12-14", "nights": 3,
      "price": 62.0, "prev_price": 65.0, "price_change": -3.0, "currency": "EUR",
      "price_level": "low",
      "airline": "Ryanair", "departure": "8:30 PM on Fri, Dec 11", "arrival": "8:55 PM on Fri, Dec 11",
      "duration": "1 hr 25 min", "stops": 0, "stops_text": "directo",
      "checked_at": "2026-11-18T11:31:02+01:00",
      "google_flights_url": "https://www.google.com/travel/flights?tfs=...&hl=es&curr=EUR",
      "hotel": {
        "hotel_id": "ChkI...", "name": "Lisboa Parque Guest House", "price_total": 36.0,
        "price_night": 12.0, "currency": "EUR", "rating": 3.5, "reviews": 321, "stars": null,
        "hostel": false, "url": "https://www.google.com/travel/hotels/entity/.../prices?ts=..."
      },
      "hotels_checked": true,
      "trip_total": 98.0,
      "google_hotels_url": "https://www.google.com/travel/search?q=hoteles%20en%20Lisboa&ts=..."
    }
  ],
  "cheapest_by_destination": [
    {"destination": "LIS", "city": "Lisboa", "country": "Portugal", "price": 62.0, "currency": "EUR",
     "departure_date": "2026-12-11", "return_date": "2026-12-14", "nights": 3, "airline": "Ryanair",
     "google_flights_url": "https://...", "is_deal": true}
  ]
}
```
Parámetros de alojamiento (opcionales): `hotel_min_rating` (Double, defecto 3.5), `hotel_min_reviews` (Int, defecto 20), `include_hostels` (Boolean, defecto true), `max_total` (Double, máximo vuelo + alojamiento), `sort` (`"price"` por vuelo o `"total"` por vuelo + alojamiento).

Notas: `hotel` es el alojamiento más barato que cumple los filtros (precio **total** de todas las noches, por persona); puede ser `null` y entonces `trip_total` también. `hotels_checked: false` significa que aún no se ha buscado alojamiento para esa oferta (mostrar "Buscar alojamiento" abriendo `google_hotels_url`). `hostel: true` = albergue (cama en habitación compartida). `rating`, `reviews` y `stars` pueden ser `null`. `prev_price` y `price_change` pueden ser `null`; `stops` puede ser `null`; `price_level` puede ser `null` (`"low"`, `"typical"`, `"high"`). `departure`/`arrival` vienen en inglés: muestra solo la hora, lo que hay antes de `" on "`. Lo precalculado cubre salidas en jueves, viernes y sábado, 2-4 noches, próximas 8 semanas. Si lo pedido queda fuera, `coverage_ok` es `false` y `coverage_notes` explica por qué (ej. `"Días de salida no precalculados: lun"`).

### B) Búsqueda en vivo (solo si `coverage_ok == false`)

1. `POST api/deals` con cuerpo JSON (campos opcionales; no envíes los nulos, Gson los omite por defecto):
   ```json
   {"origin": "AGP", "max_price": 100, "date_from": "2026-12-01", "date_to": "2026-12-28",
    "min_nights": 2, "max_nights": 3, "departure_weekdays": [4], "step_days": 7,
    "destinations": ["LON", "ROM"], "max_stops": null}
   ```
   Respuesta `202`: `{"job_id": "aef5c671c90f", "status": "running", "total_queries": 280, "estimated_seconds": 121, "status_url": "/api/deals/aef5c671c90f"}`
2. `GET api/deals/{job_id}?limit=100` cada 3 s hasta que `status != "running"`:
   ```json
   {"job_id": "aef5c671c90f", "status": "running",
    "progress": {"done": 120, "total": 280, "errors": 3, "percent": 42.9},
    "deals_count": 12, "deals": [ /* mismos campos que en latest, sin prev_price/price_change/checked_at */ ],
    "cheapest_by_destination": [ /* igual que en latest */ ],
    "no_results": []}
   ```
   `status` final: `"done"` o `"cancelled"`. Los resultados parciales llegan mientras corre: muéstralos.
3. `DELETE api/deals/{job_id}` para cancelar (cuando el usuario sale de la pantalla o pulsa Cancelar).

Puede tardar de 15 s a varios minutos: barra de progreso con `progress.percent` y botón Cancelar.

### C) Otros (opcional, segunda pantalla)

- `GET api/deals/capitals` → `[{"code": "LON", "city": "Londres", "country": "Reino Unido"}, ...]` (para un selector de destinos).
- `GET api/hotels?destination=LIS&checkin=2026-12-11&checkout=2026-12-14` (opcionales: `min_rating`, `min_reviews`, `max_price_night`, `include_hostels`, `limit`, `refresh`) → lista de alojamientos de esa oferta, para una pantalla de detalle:
  ```json
  {"destination": "LIS", "city": "Lisboa", "checkin": "2026-12-11", "checkout": "2026-12-14", "nights": 3,
   "source": "precomputed", "checked_at": "2026-11-23T10:40:12+01:00",
   "google_hotels_url": "https://...", "found": 18, "count": 12,
   "hotels": [ /* mismos campos que "hotel" en latest */ ]}
  ```
  Si no hay datos guardados busca en vivo (3-10 s): mostrar un indicador de carga.

## 3. Errores

- `401`: clave incorrecta → mensaje "Clave de API no válida".
- `422`: parámetros inválidos. `detail` puede ser **una lista de strings** (`["El escaneo necesita 560 consultas (máximo 400). Reduce..."]`) **o una lista de objetos** con campo `msg` (validación de FastAPI: `[{"loc": [...], "msg": "Value error, date_from no puede estar en el pasado", "type": "value_error"}]`). Parsea ambos casos y muestra los mensajes al usuario.
- `429`: demasiados escaneos en curso → "Servidor ocupado, inténtalo en un minuto".
- `502`: Google no devolvió precios → "No se pudieron obtener precios ahora".
- `IOException` / timeout → "Sin conexión con el servidor" (con botón Reintentar).

Modela el estado de la pantalla con una `sealed interface` (Cargando, Datos, BuscandoEnVivo(progreso), Error(mensaje)).

## 4. Pantalla (Compose, Material 3)

- Filtros arriba: precio máximo (Slider 30-300 €, por defecto 100), chips de día de salida (L M X J V S D; por defecto V), rango de noches (2-3 por defecto), destinos opcionales.
- Botón **Buscar**: llama a `latest`. Si `coverage_ok` es `true` muestra la lista y "Actualizado hace X min" (de `age_minutes`). Si es `false`, muestra `coverage_notes` y un botón **Buscar en vivo** que lanza el flujo B.
- Filtros de alojamiento: interruptor "Incluir albergues" y orden "Por vuelo / Por viaje completo" (`sort`).
- Lista (`LazyColumn`) de tarjetas: precio grande, ciudad + país, fechas formateadas en español ("vie 11/12 → lun 14/12"), noches, aerolínea, horario de ida, duración, escalas. Si `price_change < 0`, una etiqueta verde "↓ 3 €". Debajo, una fila de alojamiento: nombre, `price_total` (y `price_night`/noche), valoración con estrella, etiqueta "Albergue" si `hostel`; y destacado el **total del viaje** (`trip_total`).
- Al tocar la parte del vuelo: abrir `google_flights_url`; al tocar la del alojamiento: abrir `hotel.url` (o `google_hotels_url` si `hotel` es `null`), con `Intent.ACTION_VIEW`. Un botón "Más alojamientos" abre la pantalla de detalle con `api/hotels`.
- Pull-to-refresh para repetir la consulta.

## 5. Entregables

Lista de archivos con su contenido completo: dependencias en `gradle/libs.versions.toml` y `app/build.gradle.kts`, `AndroidManifest.xml`, `network_security_config.xml`, modelos de datos (con `@SerializedName` para pasar de snake_case a camelCase), `FlightsApi.kt` (interfaz Retrofit), `ApiClient.kt` (OkHttp + interceptor + Retrofit), `DealsRepository.kt`, `DealsViewModel.kt`, `DealsScreen.kt`. Explica dónde poner `FLIGHTS_API_KEY` en `local.properties`.

Antes de programar, indícame cómo comprobar que el móvil llega al servidor: abrir `http://150.214.109.26:8000/health` en el navegador del móvil/emulador.
