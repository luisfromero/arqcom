# Seguimiento de precios de vuelos (Google Flights + fast-flights)

API REST (pensada para una app Android) que consulta Google Flights con
[`fast-flights`](https://github.com/AWeirdDev/flights). Además, vigila a diario una ruta, guarda el
mínimo del día en `./data/prices.csv` y avisa por webhook (n8n, Discord, Telegram…) cuando el precio
baja de un umbral.

## Archivos

| Archivo | Para qué sirve |
|---|---|
| `Dockerfile` | `python:3.11-slim-bookworm` + Chromium de Playwright (desactivable) |
| `docker-compose.yml` | Toda la configuración en variables de entorno; publica el puerto 8000 |
| `requirements.txt` | Versiones fijadas (`fast-flights==2.2`, `primp==0.14.0`, FastAPI…) |
| `api.py` | API REST (FastAPI) + seguimiento diario en segundo plano |
| `deals.py` | Escaneo de ofertas ida y vuelta a capitales europeas (`/api/deals`) |
| `precompute.py` | Precálculo programado en SQLite (`/api/deals/latest`, respuesta instantánea) |
| `flujo_n8n.json` | Flujo de n8n que muestra las ofertas como página web |
| `flight_tracker.py` | Consulta, log, historial CSV y webhook (también se puede usar desde línea de comandos o cron) |

## Uso rápido

```bash
cp .env.example .env   # y pon tu clave en API_KEY (.env no se sube a git)
# Edita ORIGIN, DESTINATION, DEPARTURE_DATE, PRICE_THRESHOLD... en docker-compose.yml
docker compose up -d --build     # API en :8000 + consulta al arrancar y cada día a SCHEDULE_TIME
docker compose logs -f           # ver resultados y peticiones
cat data/prices.csv              # historial
```

Documentación interactiva (Swagger) para probar la API: <http://localhost:8000/docs>
(pulsa **Authorize** e introduce la clave).

Ejemplo de log de la consulta diaria:

```
INFO    AGP -> LHR | solo ida 2026-12-15 | 1 adulto(s) | economy
INFO    Nivel de precio según Google: LOW
INFO    PRECIO    AEROLÍNEA                HORARIO                DURACIÓN     ESCALAS
INFO    67 EUR    Vueling                  6:40 AM - 8:05 PM      14 hr 25 min 1 escala
INFO    86 EUR    Vueling                  2:00 PM - 8:05 PM      7 hr 5 min   1 escala
INFO    Respecto a la última consulta: +0 EUR | mínimo histórico: 67 EUR
INFO    ¡PRECIO BAJO UMBRAL! 67 < 80 EUR
INFO    Webhook enviado (HTTP 200)
```

## API REST

Todas las rutas `/api/*` exigen la cabecera `X-API-Key` con el valor de `API_KEY`
(si `API_KEY` está vacía, la API queda abierta).

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/health` | Estado (sin clave) |
| GET | `/api/search` | Búsqueda en vivo. Parámetros: `origin`, `destination`, `departure_date` (obligatorios), `return_date`, `adults`, `seat_class`, `max_stops`, `currency`, `limit` (1-50), `refresh` |
| GET | `/api/tracker` | Ruta en seguimiento, umbral, hora y último resultado |
| POST | `/api/tracker/check` | Lanza ahora la comprobación diaria (guarda historial y envía webhook) |
| GET | `/api/history` | Historial de mínimos. Sin parámetros, el de la ruta seguida; filtrable por `origin`, `destination`, `departure_date`, `return_date` |
| POST | `/api/deals` | Lanza un escaneo de ofertas ida y vuelta a capitales europeas (ver abajo) |
| GET | `/api/deals/{job_id}` | Progreso y resultados del escaneo (parciales mientras corre) |
| DELETE | `/api/deals/{job_id}` | Cancela un escaneo |
| GET | `/api/deals/capitals` | Capitales que se prueban por defecto |

Códigos de error: `401` (clave), `422` (parámetros inválidos; `detail` es una lista de mensajes),
`429` (demasiados escaneos a la vez), `502` (Google no devolvió precios).

```bash
curl -H "X-API-Key: $API_KEY" \
  "http://localhost:8000/api/search?origin=AGP&destination=CDG&departure_date=2026-12-15&return_date=2026-12-20&limit=2"
```

```json
{
  "query": {"origin": "AGP", "destination": "CDG", "departure_date": "2026-12-15",
            "return_date": "2026-12-20", "adults": 1, "seat_class": "economy",
            "max_stops": null, "currency": "EUR"},
  "checked_at": "2026-11-16T10:23:02+01:00",
  "price_level": "typical", "fetch_mode": "common",
  "min_price": 212.0, "currency": "EUR", "total_results": 25, "cached": false,
  "flights": [
    {"airline": "Air France", "price": 212.0, "currency": "EUR",
     "departure": "1:00 PM on Tue, Dec 15", "arrival": "3:40 PM on Tue, Dec 15",
     "arrival_time_ahead": "", "duration": "2 hr 40 min", "stops": 0,
     "stops_text": "directo", "is_best": true, "delay": null}
  ]
}
```

- Las búsquedas idénticas se sirven de caché durante `CACHE_TTL_SECONDS` (15 min por defecto);
  `refresh=true` la ignora. Así la app no satura a Google.
- Las consultas a Google se hacen de una en una. Con `common` tardan ~1-2 s; si hay que recurrir a
  Playwright pueden tardar 10-60 s, así que usa un timeout de lectura amplio en la app.
- En ida y vuelta, el precio es el total del viaje y los horarios son los del vuelo de ida.

## Ofertas a capitales europeas (fechas abiertas)

Busca ida y vuelta desde Málaga (configurable) a 35 capitales europeas por debajo de un precio.
Google Flights no da (con esta librería) el calendario completo de precios, así que la API
**prueba combinaciones** de destino × fecha de salida × noches. Como son muchas consultas, funciona
como un trabajo en segundo plano:

1. `POST /api/deals` con los filtros → responde `202` con `job_id`, `total_queries` y `estimated_seconds`.
2. `GET /api/deals/{job_id}` cada pocos segundos → `status` (`running` / `done` / `cancelled`),
   `progress` y resultados parciales.

Cuerpo JSON (todos los campos son opcionales):

| Campo | Defecto | Descripción |
|---|---|---|
| `origin` | `AGP` (`DEALS_ORIGIN`) | Aeropuerto de salida |
| `max_price` | `100` | Precio máximo ida y vuelta por persona |
| `date_from` | hoy + 7 días | Primera fecha de salida |
| `date_to` | `date_from` + 4 semanas (27 días) | Última fecha de salida (máx. 180 días de ventana) |
| `min_nights`, `max_nights` | `2`, `3` | Duración de la estancia |
| `departure_weekdays` | — | Días de salida: `0`=lunes … `6`=domingo. `[4]` = solo viernes |
| `step_days` | `7` | Si no hay `departure_weekdays`, una salida cada N días |
| `destinations` | todas | Lista de códigos (`["LON","ROM","LIS"]`) |
| `adults`, `seat_class`, `max_stops`, `currency` | `1`, `economy`, —, `EUR` | Como en `/api/search` |

Nº de consultas = destinos × fechas de salida × noches posibles. Si supera `DEALS_MAX_QUERIES`
(400) responde `422` explicando cuántas harían falta. Referencia medida: 280 consultas ≈ 2¼ min.

Ejemplo, escapadas de fin de semana (salida en viernes, 2-3 noches) durante el próximo mes:

```bash
curl -X POST -H "X-API-Key: $API_KEY" -H "Content-Type: application/json" \
  -d '{"max_price": 100, "departure_weekdays": [4], "min_nights": 2, "max_nights": 3}' \
  http://localhost:8000/api/deals
# {"job_id":"7beaa689c14e","status":"running","total_queries":280,"estimated_seconds":121,...}

curl -H "X-API-Key: $API_KEY" "http://localhost:8000/api/deals/7beaa689c14e?limit=20"
```

Respuesta (resumida, resultado real del 16-11-2026):

```json
{
  "job_id": "7beaa689c14e", "status": "done",
  "progress": {"done": 280, "total": 280, "errors": 8, "percent": 100.0},
  "deals_count": 27,
  "deals": [
    {"destination": "LON", "city": "Londres", "country": "Reino Unido",
     "departure_date": "2026-12-11", "return_date": "2026-12-13", "nights": 2,
     "price": 54.0, "currency": "EUR", "price_level": "typical",
     "airline": "Ryanair", "departure": "6:50 PM on Fri, Dec 11",
     "arrival": "8:45 PM on Fri, Dec 11", "duration": "2 hr 55 min",
     "stops": 0, "stops_text": "directo",
     "google_flights_url": "https://www.google.com/travel/flights?tfs=...&hl=es&curr=EUR"}
  ],
  "cheapest_by_destination": [
    {"destination": "LON", "city": "Londres", "price": 54.0, "is_deal": true, ...},
    {"destination": "KEF", "city": "Reikiavik", "price": 427.0, "is_deal": false, ...}
  ],
  "no_results": ["SKP"]
}
```

- `deals`: todas las combinaciones por debajo de `max_price`, ordenadas por precio.
- `cheapest_by_destination`: lo más barato encontrado para cada capital, sea oferta o no.
- `no_results`: destinos sin ningún precio (desde Málaga, Skopie no devuelve vuelos en Google).
- `google_flights_url`: abre esa misma búsqueda en Google Flights para reservar.
- Se usan códigos de ciudad (`LON`, `PAR`, `ROM`, `STO`) para incluir todos sus aeropuertos
  (Stansted, Beauvais, Ciampino…), lo que abarata mucho: AGP→LHR 286 € frente a AGP→LON 113 €.
- Los escaneos se guardan en memoria 1 hora y se pierden al reiniciar el contenedor.

## Ofertas precalculadas (respuesta instantánea)

Para que la app no tenga que esperar, la API escanea por su cuenta una rejilla amplia dos veces al
día y guarda en SQLite (`./data/deals.db`) el precio de **cada** combinación, no solo las ofertas.
Cualquier filtro se resuelve después con una consulta a la base de datos, en milisegundos.

Rejilla por defecto: 35 capitales × salidas en **jueves, viernes y sábado** de las próximas
**8 semanas** × **2-4 noches** = 2.520 consultas (~30 min), a las **06:00 y 18:00**. Si al
arrancar el contenedor el último escaneo tiene más de 12 h, lanza uno inmediatamente.

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/deals/latest` | Ofertas precalculadas. Parámetros: `max_price` (100), `weekdays` (`4` o `3,4,5`), `min_nights`, `max_nights`, `date_from`, `date_to`, `destinations` (`LON,ROM`), `max_stops`, `max_age_hours` (36), `limit` |
| GET | `/api/deals/scans` | Escaneo en curso (progreso) y últimos escaneos |
| POST | `/api/deals/scans` | Lanzar un escaneo ahora (`409` si ya hay uno) |

```bash
curl -H "X-API-Key: $API_KEY" "http://localhost:8000/api/deals/latest?max_price=80&weekdays=4&min_nights=2&max_nights=3"
```

```json
{
  "source": "precomputed",
  "updated_at": "2026-11-18T11:45:10+01:00", "age_minutes": 12,
  "scan_running": null,
  "coverage": {"origin": "AGP", "date_from": "2026-11-20", "date_to": "2027-01-14",
               "weekdays": [3, 4, 5], "min_nights": 2, "max_nights": 4},
  "coverage_ok": true, "coverage_notes": [],
  "deals_count": 41,
  "deals": [
    {"destination": "LON", "city": "Londres", "departure_date": "2026-12-11",
     "return_date": "2026-12-13", "nights": 2, "price": 54.0, "prev_price": 57.0,
     "price_change": -3.0, "airline": "Ryanair", "checked_at": "2026-11-18T11:31:02+01:00",
     "google_flights_url": "https://www.google.com/travel/flights?...", "...": "..."}
  ],
  "cheapest_by_destination": [{"destination": "LON", "price": 54.0, "is_deal": true, "...": "..."}]
}
```

- `coverage_ok: false` (con `coverage_notes` explicando por qué) significa que lo pedido queda
  fuera de la rejilla (otro día de salida, más noches, fechas más lejanas, otro destino u origen).
  En ese caso la app debería lanzar el escaneo en vivo con `POST /api/deals`.
- `prev_price` / `price_change`: precio del escaneo anterior y diferencia (negativo = ha bajado).
- No se sirven precios con más de `max_age_hours` horas, y las salidas ya pasadas se borran.
- La rejilla se configura con las variables `PRECOMPUTE_*` (ver tabla de variables).

## Cliente Android

### URL del servidor

| Dónde corre la app | URL base |
|---|---|
| Emulador de Android Studio (mismo PC que Docker) | `http://10.0.2.2:8000/` |
| Móvil real en la misma red | `http://<IP-del-PC>:8000/` (`ipconfig` en Windows) |
| Fuera de tu red | Detrás de HTTPS: proxy inverso (Caddy/nginx) o túnel (Cloudflare Tunnel, ngrok) |

En Windows, para que el móvil llegue al PC, abre el puerto en el firewall (PowerShell como administrador):

```powershell
New-NetFirewallRule -DisplayName "Flight API 8000" -Direction Inbound -Protocol TCP -LocalPort 8000 -Action Allow -Profile Private
```

> Si el PC tiene IP pública (p. ej. en la red de la universidad), el puerto 8000 puede quedar visible
> desde Internet: cambia `API_KEY` por una clave larga y aleatoria.

### `AndroidManifest.xml`

```xml
<uses-permission android:name="android.permission.INTERNET" />

<application
    android:usesCleartextTraffic="true"  ... >  <!-- necesario para http:// (sin TLS) -->
```

### `build.gradle.kts` (módulo app)

```kotlin
dependencies {
    implementation("com.squareup.retrofit2:retrofit:2.11.0")
    implementation("com.squareup.retrofit2:converter-gson:2.11.0")
    implementation("com.squareup.okhttp3:okhttp:4.12.0")
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-android:1.8.1")
}
```

### `FlightApi.kt`

```kotlin
import okhttp3.OkHttpClient
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.http.*
import java.util.concurrent.TimeUnit

data class Flight(
    val airline: String, val price: Double, val currency: String,
    val departure: String, val arrival: String, val arrival_time_ahead: String,
    val duration: String, val stops: Int?, val stops_text: String,
    val is_best: Boolean, val delay: String?
)

data class SearchResponse(
    val checked_at: String, val price_level: String?, val min_price: Double,
    val currency: String, val total_results: Int, val cached: Boolean,
    val flights: List<Flight>
)

data class HistoryRecord(
    val checked_at: String, val min_price: Double?, val price_level: String?,
    val airline: String, val stops: Int?
)

data class HistoryResponse(
    val count: Int, val lowest: Double?, val highest: Double?,
    val records: List<HistoryRecord>
)

// Ofertas: los campos null no se envían (Gson los omite) y la API usa su valor por defecto
data class DealsRequest(
    val origin: String = "AGP", val max_price: Double = 100.0,
    val date_from: String? = null, val date_to: String? = null,       // "2026-12-01"
    val min_nights: Int = 2, val max_nights: Int = 3,
    val departure_weekdays: List<Int>? = null,                        // listOf(4) = viernes
    val step_days: Int = 7, val destinations: List<String>? = null
)

data class DealsJobCreated(val job_id: String, val total_queries: Int, val estimated_seconds: Int)

data class Progress(val done: Int, val total: Int, val errors: Int, val percent: Double)

data class Deal(
    val destination: String, val city: String, val country: String,
    val departure_date: String, val return_date: String, val nights: Int,
    val price: Double, val currency: String, val airline: String,
    val departure: String, val arrival: String, val duration: String,
    val stops: Int?, val stops_text: String, val google_flights_url: String,
    val prev_price: Double? = null, val price_change: Double? = null   // solo en /latest
)

data class DealsStatus(
    val job_id: String, val status: String, val progress: Progress,
    val deals_count: Int, val deals: List<Deal>, val no_results: List<String>
)

data class LatestDeals(
    val updated_at: String?, val age_minutes: Int?,
    val coverage_ok: Boolean, val coverage_notes: List<String>,
    val deals_count: Int, val deals: List<Deal>
)

interface FlightApi {
    @GET("api/search")
    suspend fun search(
        @Query("origin") origin: String,
        @Query("destination") destination: String,
        @Query("departure_date") departureDate: String,        // "2026-12-15"
        @Query("return_date") returnDate: String? = null,      // null = solo ida
        @Query("adults") adults: Int = 1,
        @Query("seat_class") seatClass: String = "economy",
        @Query("max_stops") maxStops: Int? = null,
        @Query("limit") limit: Int = 20
    ): SearchResponse

    @GET("api/history")
    suspend fun history(): HistoryResponse

    @POST("api/tracker/check")
    suspend fun checkNow(): Map<String, Any?>

    @POST("api/deals")
    suspend fun startDeals(@Body req: DealsRequest): DealsJobCreated

    @GET("api/deals/latest")
    suspend fun latestDeals(
        @Query("max_price") maxPrice: Double = 100.0,
        @Query("weekdays") weekdays: String? = null,          // "4" o "3,4,5"
        @Query("min_nights") minNights: Int? = null,
        @Query("max_nights") maxNights: Int? = null,
        @Query("date_from") dateFrom: String? = null,
        @Query("date_to") dateTo: String? = null,
        @Query("destinations") destinations: String? = null,  // "LON,ROM"
        @Query("limit") limit: Int = 100
    ): LatestDeals

    @GET("api/deals/{id}")
    suspend fun deals(@Path("id") jobId: String, @Query("limit") limit: Int = 100): DealsStatus

    @DELETE("api/deals/{id}")
    suspend fun cancelDeals(@Path("id") jobId: String): Map<String, Any?>

    companion object {
        fun create(baseUrl: String, apiKey: String): FlightApi {
            val client = OkHttpClient.Builder()
                .addInterceptor { chain ->
                    chain.proceed(chain.request().newBuilder()
                        .header("X-API-Key", apiKey).build())
                }
                .readTimeout(90, TimeUnit.SECONDS)   // Playwright puede tardar
                .build()
            return Retrofit.Builder()
                .baseUrl(baseUrl)                    // debe terminar en "/"
                .client(client)
                .addConverterFactory(GsonConverterFactory.create())
                .build()
                .create(FlightApi::class.java)
        }
    }
}
```

### Uso (por ejemplo, en un `ViewModel`)

```kotlin
private val api = FlightApi.create("http://10.0.2.2:8000/", "TU_API_KEY")

fun buscar() = viewModelScope.launch {
    try {
        val r = api.search("AGP", "LHR", "2026-12-15")
        Log.i("Vuelos", "Mínimo ${r.min_price} ${r.currency} (${r.price_level})")
        r.flights.forEach { Log.i("Vuelos", "${it.price} ${it.airline} ${it.stops_text}") }
    } catch (e: retrofit2.HttpException) {
        Log.e("Vuelos", "HTTP ${e.code()}: ${e.response()?.errorBody()?.string()}")
    } catch (e: java.io.IOException) {
        Log.e("Vuelos", "Sin conexión con la API: ${e.message}")
    }
}

// Recomendado: primero lo precalculado (instantáneo); solo si no lo cubre, escaneo en vivo
fun ofertas(maxPrice: Double, weekdays: String?, minN: Int, maxN: Int) = viewModelScope.launch {
    val r = api.latestDeals(maxPrice, weekdays, minN, maxN)
    if (r.coverage_ok) {
        ofertas.value = r.deals
        actualizado.value = "Actualizado hace ${r.age_minutes} min"
    } else {
        Log.i("Vuelos", "En vivo porque: ${r.coverage_notes}")
        buscarOfertas()                                    // el escaneo en vivo de abajo
    }
}

// Escaneo en vivo: ofertas de fin de semana a capitales por menos de 100 €, con progreso
fun buscarOfertas() = viewModelScope.launch {
    val job = api.startDeals(DealsRequest(max_price = 100.0, departure_weekdays = listOf(4),
                                          min_nights = 2, max_nights = 3))
    var estado: DealsStatus
    do {
        delay(3000)
        estado = api.deals(job.job_id)
        progreso.value = estado.progress.percent          // p. ej. un StateFlow para la barra
        ofertas.value = estado.deals                       // resultados parciales
    } while (estado.status == "running")
}

// Abrir una oferta en Google Flights para reservar
fun abrir(context: Context, deal: Deal) =
    context.startActivity(Intent(Intent.ACTION_VIEW, Uri.parse(deal.google_flights_url)))
```

## Publicar detrás de un proxy (`https://casium.uma.es/vuelos-api`)

El proxy debe quitar el prefijo antes de pasar la petición a la API, y la API debe saber cuál era para que `/docs` cargue `/vuelos-api/openapi.json`:

Con Caddy (`Caddyfile`), `handle_path` quita el prefijo:

```caddy
casium.uma.es {
    redir /vuelos-api /vuelos-api/ 308
    handle_path /vuelos-api/* {
        reverse_proxy 150.214.109.26:8000
    }
}
```

En `docker-compose.yml`: `ROOT_PATH: "/vuelos-api"` y `docker compose up -d`. El acceso directo por `http://150.214.109.26:8000/` sigue funcionando para la app; solo el Swagger de esa URL apuntaría al prefijo.

## Variables de entorno

| Variable | Defecto | Descripción |
|---|---|---|
| `API_KEY` | vacío | Clave que la app envía en `X-API-Key` (vacía = sin autenticación) |
| `CACHE_TTL_SECONDS` | `900` | Caché de `/api/search` y de cada consulta de `/api/deals` |
| `ROOT_PATH` | vacío | Prefijo público si la API va detrás de un proxy que lo elimina (ej. `/vuelos-api`); hace que `/docs` funcione |
| `DEALS_ORIGIN` | `AGP` | Origen por defecto de `/api/deals` |
| `DEALS_WORKERS` | `3` | Consultas en paralelo durante un escaneo |
| `DEALS_MAX_QUERIES` | `400` | Límite de consultas por escaneo |
| `DEALS_FETCH_MODE` | `common` | Modo de obtención en escaneos (Playwright sería muy lento) |
| `PRECOMPUTE_ENABLED` | `true` | Activar el precálculo |
| `PRECOMPUTE_TIMES` | `06:00,18:00` | Horas de los escaneos |
| `PRECOMPUTE_WEEKDAYS` | `3,4,5` | Días de salida (0=lun … 6=dom) |
| `PRECOMPUTE_NIGHTS` | `2-4` | Noches de estancia |
| `PRECOMPUTE_WEEKS` | `8` | Horizonte de salidas |
| `PRECOMPUTE_START_DAYS` | `2` | Primera salida: hoy + N días |
| `PRECOMPUTE_STALE_HOURS` | `12` | Al arrancar, escanear si el último es más viejo |
| `PRECOMPUTE_MAX_AGE_HOURS` | `36` | No servir precios más viejos |
| `PRECOMPUTE_DB` | `/data/deals.db` | Base de datos SQLite |
| `PORT` | `8000` | Puerto interno de la API |
| `ORIGIN`, `DESTINATION` | — | Ruta en seguimiento diario (códigos IATA). Vacías = solo búsquedas por API |
| `DEPARTURE_DATE` | — | `YYYY-MM-DD` de la ruta en seguimiento |
| `RETURN_DATE` | vacío | Vacío = solo ida; con fecha = ida y vuelta |
| `ADULTS` | `1` | Número de adultos |
| `SEAT_CLASS` | `economy` | `economy`, `premium-economy`, `business`, `first` |
| `MAX_STOPS` | vacío | Vacío = sin límite, `0` = solo directos |
| `CURRENCY` | `EUR` | Moneda pedida a Google |
| `PRICE_THRESHOLD` | vacío | Alerta si el mínimo es **inferior** a este valor |
| `WEBHOOK_URL` | vacío | Destino del POST JSON |
| `WEBHOOK_MODE` | `alert` | `alert` = solo bajo umbral; `always` = también resumen diario |
| `TELEGRAM_CHAT_ID` | vacío | Necesario si el webhook es la API de Telegram |
| `FETCH_MODE` | `auto` | Ver abajo |
| `SCHEDULE_TIME` | `08:00` | Hora diaria (zona `TZ`) |
| `RUN_ON_START` | `true` | Consultar también al arrancar |
| `RUN_MODE` | `schedule` | Solo con `python flight_tracker.py`: `once` = una consulta y salir |
| `TOP_N` | `5` | Vuelos que se muestran en el log |
| `DATA_FILE` | `/data/prices.csv` | Historial (volumen `./data`) |
| `GOOGLE_SOCS` | vacío | Cookie de consentimiento alternativa (ver Problemas) |
| `LOCAL_TIMEOUT_MS` | `60000` | Tiempo máximo de espera de Playwright |

### Modos de obtención (`FETCH_MODE`)

- `common`: petición HTTP directa imitando a Chrome (rápida, sin navegador).
- `local`: Chromium headless con Playwright dentro del contenedor.
- `auto` (recomendado): `common` y, si falla, `local`.
- `fallback` / `force-fallback`: el servicio remoto de la librería (`try.playwright.tech`).
  **En noviembre de 2026 ese servicio exige un token anti-bot y responde 401**, por eso no está en `auto`.

Comprobado el 16-11-2026: `common` y `local` funcionan tanto para solo ida como para ida y vuelta.
`local` usa primero el HTML que sirve Google y, si no trae vuelos, espera al renderizado JS.

## Webhook

El cuerpo se adapta a la URL:

- **Discord** (`discord.com/api/webhooks/...`) → `{"content": "<mensaje>"}`
- **Telegram** (`https://api.telegram.org/bot<TOKEN>/sendMessage`) → `{"chat_id": TELEGRAM_CHAT_ID, "text": "<mensaje>"}`
- **Cualquier otro** (n8n, Make, Home Assistant…) → JSON completo:

```json
{"event": "price_alert", "checked_at": "2026-11-16T10:14:24+01:00",
 "origin": "AGP", "destination": "LHR", "departure_date": "2026-12-15", "return_date": "",
 "min_price": 67.0, "currency": "EUR", "threshold": 80.0, "below_threshold": true,
 "price_level": "low", "airline": "Vueling", "departure": "6:40 AM on Tue, Dec 15",
 "arrival": "8:05 PM on Tue, Dec 15", "duration": "14 hr 25 min", "stops": 1,
 "stops_text": "1 escala", "previous_min": 67.0, "message": "✈️ ALERTA DE PRECIO\n..."}
```

`event` vale `price_alert` o `daily_summary` (este último solo con `WEBHOOK_MODE=always`).

## Programación diaria

### Opción A: dentro de la API (por defecto)

`docker compose up -d`: la API arranca un hilo con la librería `schedule`
(`restart: unless-stopped`). La hora la marcan `SCHEDULE_TIME` y `TZ`. Desde la app también se
puede forzar con `POST /api/tracker/check`.

### Opción B: cron del sistema

Ejecución puntual sin API (sale con código 0 si todo fue bien, 1 si falló):

```bash
docker compose run --rm -e RUN_MODE=once flights python flight_tracker.py
```

`crontab -e` en el host (cada día a las 08:00):

```cron
0 8 * * * cd /ruta/a/sesion_10_flights && /usr/bin/docker compose run --rm -e RUN_MODE=once flights python flight_tracker.py >> /var/log/flights.log 2>&1
```

En Windows se puede usar el Programador de tareas con el mismo comando. Si usas cron, deja
`ORIGIN` vacío en el servicio de la API para no hacer la consulta dos veces.

## Imagen sin navegador

Si basta con el modo `common`, la imagen es mucho más pequeña (con Chromium ocupa ~1,6 GB):

```bash
docker compose build --build-arg INSTALL_PLAYWRIGHT=false
```

y pon `FETCH_MODE: "common"`.

## Problemas conocidos

- **Página de consentimiento de cookies (UE)**: desde IPs europeas Google redirige a
  `consent.google.com`. El script envía una cookie `SOCS` que lo evita. La que genera
  `fast_flights.Cookies.new()` ya no funciona; si Google invalida la incluida, copia el valor de la
  cookie `SOCS` de tu navegador en `GOOGLE_SOCS`.
- **`Invalid impersonate: "chrome_126"`**: `fast-flights 2.2` no es compatible con `primp >= 1.0`;
  por eso `primp` está fijado a `0.14.0`.
- `fast-flights` lee el HTML de Google; si Google cambia su marcado, la consulta dejará de funcionar
  hasta que se actualice la librería.
