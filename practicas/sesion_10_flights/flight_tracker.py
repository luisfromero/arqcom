"""Seguimiento diario de precios de vuelos en Google Flights con fast-flights.

Toda la configuración llega por variables de entorno (ver docker-compose.yml).
"""

import csv
import logging
import os
import re
import sys
import threading
import time
from datetime import datetime
from pathlib import Path

from urllib.parse import urlencode

import requests
import schedule
from fast_flights import FlightData, Passengers, create_filter, get_flights_from_filter
from fast_flights.core import parse_response

log = logging.getLogger("flights")
_browser_lock = threading.Lock()

SEAT_CLASSES = {"economy", "premium-economy", "business", "first"}
# common / local: fetchers propios de este script (con cookies de consentimiento UE).
# fallback / force-fallback: servicio remoto de la librería (try.playwright.tech).
FETCH_MODES = {"common", "local", "fallback", "force-fallback"}
AUTO_CHAIN = ["common", "local"]
GOOGLE_FLIGHTS = "https://www.google.com/travel/flights"
LOCAL_TIMEOUT_MS = int(os.getenv("LOCAL_TIMEOUT_MS", "60000"))
DEFAULT_SOCS = "CAISHAgBEhJnd3NfMjAyMzA4MTAtMF9SQzIaAmVuIAEaBgiAo_CmBg"
CHROME_UA = ("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
             "(KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36")

CSV_FIELDS = [
    "checked_at", "origin", "destination", "departure_date", "return_date",
    "adults", "seat_class", "min_price", "currency", "price_level", "airline",
    "departure", "arrival", "duration", "stops", "fetch_mode",
]


# --------------------------------------------------------------------------- #
# Configuración
# --------------------------------------------------------------------------- #
def env(name, default=""):
    value = os.getenv(name, "").strip()
    return value if value else default


def validate_search(s):
    """Valida los campos de una búsqueda. Devuelve una lista de errores (vacía si es válida)."""
    errors = []
    for key in ("origin", "destination", "departure_date"):
        if not s.get(key):
            errors.append(f"{key} es obligatorio")
    for key in ("origin", "destination"):
        if s.get(key) and not re.fullmatch(r"[A-Z]{3}", s[key]):
            errors.append(f"{key} debe ser un código IATA de 3 letras")
    dates = {}
    for key in ("departure_date", "return_date"):
        if s.get(key):
            try:
                dates[key] = datetime.strptime(s[key], "%Y-%m-%d").date()
            except ValueError:
                errors.append(f"{key} debe tener formato YYYY-MM-DD")
    if "departure_date" in dates and dates["departure_date"] < datetime.now().date():
        errors.append("departure_date no puede estar en el pasado")
    if len(dates) == 2 and dates["return_date"] < dates["departure_date"]:
        errors.append("return_date debe ser posterior a departure_date")
    if not 1 <= s.get("adults", 1) <= 9:
        errors.append("adults debe estar entre 1 y 9")
    if s.get("seat_class") not in SEAT_CLASSES:
        errors.append(f"seat_class debe ser uno de {sorted(SEAT_CLASSES)}")
    if s.get("max_stops") is not None and s["max_stops"] < 0:
        errors.append("max_stops no puede ser negativo")
    return errors


def load_config(require_route=True):
    cfg = {
        "origin": env("ORIGIN").upper(),
        "destination": env("DESTINATION").upper(),
        "departure_date": env("DEPARTURE_DATE"),
        "return_date": env("RETURN_DATE"),
        "adults": int(env("ADULTS", "1")),
        "seat_class": env("SEAT_CLASS", "economy").lower(),
        "max_stops": int(env("MAX_STOPS")) if env("MAX_STOPS") else None,
        "currency": env("CURRENCY", "EUR").upper(),
        "threshold": float(env("PRICE_THRESHOLD")) if env("PRICE_THRESHOLD") else None,
        "webhook_url": env("WEBHOOK_URL"),
        "webhook_mode": env("WEBHOOK_MODE", "alert").lower(),
        "telegram_chat_id": env("TELEGRAM_CHAT_ID"),
        "fetch_mode": env("FETCH_MODE", "auto").lower(),
        "run_mode": env("RUN_MODE", "schedule").lower(),
        "schedule_time": env("SCHEDULE_TIME", "08:00"),
        "run_on_start": env("RUN_ON_START", "true").lower() in {"1", "true", "yes"},
        "top_n": int(env("TOP_N", "5")),
        "data_file": Path(env("DATA_FILE", "/data/prices.csv")),
    }

    # Sin ruta configurada (ORIGIN vacío) la API sigue funcionando, pero sin seguimiento diario.
    cfg["tracking"] = bool(cfg["origin"] or require_route)
    errors = [e.upper() for e in validate_search(cfg)] if cfg["tracking"] else []
    if cfg["fetch_mode"] != "auto" and cfg["fetch_mode"] not in FETCH_MODES:
        errors.append(f"FETCH_MODE debe ser 'auto' o uno de {sorted(FETCH_MODES)}")
    if cfg["webhook_mode"] not in {"alert", "always"}:
        errors.append("WEBHOOK_MODE debe ser 'alert' o 'always'")
    if errors:
        raise ValueError("; ".join(errors))
    return cfg


# --------------------------------------------------------------------------- #
# Consulta
# --------------------------------------------------------------------------- #
def parse_price(text):
    """'€1,234' / '1.234 €' / '$99' -> 1234.0 ; None si no hay número."""
    digits = re.sub(r"[^\d]", "", text or "")
    return float(digits) if digits else None


class HtmlResponse:
    """Respuesta mínima compatible con fast_flights.core.parse_response."""

    def __init__(self, html):
        self.status_code = 200
        self.text = html
        self.text_markdown = html[:500]


def consent_cookies():
    # Sin estas cookies, desde IPs europeas Google redirige a consent.google.com.
    # El SOCS que genera fast-flights (Cookies.new) ya no basta; este valor sí funciona
    # y se puede sustituir con la variable GOOGLE_SOCS si Google lo invalida.
    return {"CONSENT": "YES+", "SOCS": env("GOOGLE_SOCS", DEFAULT_SOCS)}


def fetch_common(params):
    """Petición HTTP directa (rápida, sin navegador) imitando a Chrome."""
    from primp import Client

    client = Client(impersonate="chrome_126", verify=False)
    cookie_header = "; ".join(f"{k}={v}" for k, v in consent_cookies().items())
    res = client.get(GOOGLE_FLIGHTS, params=params, headers={"Cookie": cookie_header})
    if res.status_code != 200:
        raise RuntimeError(f"HTTP {res.status_code}")
    if "consent.google.com" in str(res.url):
        raise RuntimeError("Google redirigió a la página de consentimiento")
    return res


def fetch_local(params):
    """Renderizado completo con Chromium headless (Playwright) dentro del contenedor."""
    from playwright.sync_api import sync_playwright

    url = f"{GOOGLE_FLIGHTS}?{urlencode(params)}"
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        try:
            context = browser.new_context(locale="en-US", user_agent=CHROME_UA)
            context.add_cookies([{"name": k, "value": v, "domain": ".google.com", "path": "/"}
                                 for k, v in consent_cookies().items()])
            page = context.new_page()
            resp = page.goto(url, wait_until="domcontentloaded", timeout=LOCAL_TIMEOUT_MS)
            if page.url.startswith("https://consent.google.com"):
                page.locator("button:has-text('Accept all'), button:has-text('Aceptar todo')"
                             ).first.click()
                resp = page.wait_for_event("response", lambda r: r.url.startswith(GOOGLE_FLIGHTS),
                                           timeout=LOCAL_TIMEOUT_MS)
            # El HTML servido ya suele traer los resultados; a veces el JS de Google
            # falla después en headless ("Oops, something went wrong"), así que se usa primero.
            raw = HtmlResponse(resp.text() if resp else "")
            try:
                parse_response(raw)
                return raw
            except RuntimeError:
                log.info("HTML inicial sin vuelos; esperando al renderizado JS...")
            page.locator("ul.Rk10dc li").first.wait_for(timeout=LOCAL_TIMEOUT_MS)
            page.wait_for_timeout(1500)  # deja terminar de pintar precios
            html = page.evaluate("() => document.querySelector('[role=\"main\"]').innerHTML")
        finally:
            browser.close()
    return HtmlResponse(html)


def build_filter(cfg):
    legs = [FlightData(date=cfg["departure_date"],
                       from_airport=cfg["origin"], to_airport=cfg["destination"])]
    if cfg["return_date"]:
        legs.append(FlightData(date=cfg["return_date"],
                               from_airport=cfg["destination"], to_airport=cfg["origin"]))

    tfs = create_filter(
        flight_data=legs,
        trip="round-trip" if cfg["return_date"] else "one-way",
        seat=cfg["seat_class"],
        passengers=Passengers(adults=cfg["adults"], children=0,
                              infants_in_seat=0, infants_on_lap=0),
        max_stops=cfg["max_stops"],
    )
    params = {"tfs": tfs.as_b64().decode(), "hl": "en", "tfu": "EgQIABABIgA",
              "curr": cfg["currency"]}
    return tfs, params


def google_flights_url(cfg, lang="es"):
    """Enlace para abrir la misma búsqueda en Google Flights (navegador / app)."""
    _, params = build_filter(cfg)
    return f"{GOOGLE_FLIGHTS}?{urlencode({**params, 'hl': lang})}"


def fetch_flights(cfg):
    tfs, params = build_filter(cfg)
    modes = AUTO_CHAIN if cfg["fetch_mode"] == "auto" else [cfg["fetch_mode"]]
    last_error = None
    for mode in modes:
        try:
            log.debug("Consultando Google Flights (modo=%s)...", mode)
            if mode == "common":
                result = parse_response(fetch_common(params))
            elif mode == "local":
                with _browser_lock:  # un Chromium cada vez (mucha RAM)
                    result = parse_response(fetch_local(params))
            else:
                result = get_flights_from_filter(tfs, currency=cfg["currency"], mode=mode)
            if result.flights:
                return result, mode
            last_error = RuntimeError("respuesta sin vuelos")
        except Exception as exc:  # noqa: BLE001 - cualquier fallo -> siguiente modo
            last_error = exc
            first_line = str(exc).strip().splitlines()[0][:200] if str(exc).strip() else ""
            log.debug("Modo %s falló: %s %s", mode, type(exc).__name__, first_line)
    raise RuntimeError(f"Todos los modos fallaron ({', '.join(modes)}): {last_error}")


# --------------------------------------------------------------------------- #
# Presentación, historial y alertas
# --------------------------------------------------------------------------- #
def describe_stops(stops):
    if stops == 0:
        return "directo"
    if isinstance(stops, int):
        return f"{stops} escala{'s' if stops > 1 else ''}"
    return "escalas ?"


def log_flights(cfg, flights, price_level, top_n):
    route = f"{cfg['origin']} -> {cfg['destination']}"
    trip = (f"ida {cfg['departure_date']} / vuelta {cfg['return_date']}"
            if cfg["return_date"] else f"solo ida {cfg['departure_date']}")
    log.info("=" * 78)
    log.info("%s | %s | %d adulto(s) | %s", route, trip, cfg["adults"], cfg["seat_class"])
    log.info("Nivel de precio según Google: %s", (price_level or "desconocido").upper())
    log.info("-" * 78)
    log.info("%-9s %-24s %-22s %-12s %s", "PRECIO", "AEROLÍNEA", "HORARIO", "DURACIÓN", "ESCALAS")
    for f, price in flights[:top_n]:
        horario = f"{f.departure.split(' on ')[0]} - {f.arrival.split(' on ')[0]}{f.arrival_time_ahead}"
        log.info("%-9s %-24.24s %-22.22s %-12.12s %s",
                 f"{price:.0f} {cfg['currency']}", f.name, horario, f.duration,
                 describe_stops(f.stops))
    log.info("=" * 78)


def read_history(path):
    if not path.exists():
        return []
    with path.open(newline="", encoding="utf-8") as fh:
        return list(csv.DictReader(fh))


def append_history(path, row):
    path.parent.mkdir(parents=True, exist_ok=True)
    new_file = not path.exists()
    with path.open("a", newline="", encoding="utf-8") as fh:
        writer = csv.DictWriter(fh, fieldnames=CSV_FIELDS)
        if new_file:
            writer.writeheader()
        writer.writerow(row)


def same_search(row, cfg):
    return (row.get("origin") == cfg["origin"] and row.get("destination") == cfg["destination"]
            and row.get("departure_date") == cfg["departure_date"]
            and row.get("return_date", "") == cfg["return_date"])


def build_message(payload):
    p = payload
    head = "✈️ ALERTA DE PRECIO" if p["below_threshold"] else "✈️ Resumen diario de vuelos"
    lines = [
        head,
        f"{p['origin']} → {p['destination']} | {p['departure_date']}"
        + (f" ↔ {p['return_date']}" if p["return_date"] else " (solo ida)"),
        f"Mínimo: {p['min_price']:.0f} {p['currency']} (nivel Google: {p['price_level'] or '?'})",
        f"{p['airline']} | {p['departure']} → {p['arrival']} | {p['duration']} | {p['stops_text']}",
    ]
    if p["threshold"] is not None:
        lines.append(f"Umbral: {p['threshold']:.0f} {p['currency']}")
    if p["previous_min"] is not None:
        lines.append(f"Mínimo histórico previo: {p['previous_min']:.0f} {p['currency']}")
    return "\n".join(lines)


def send_webhook(cfg, payload):
    url = cfg["webhook_url"]
    text = build_message(payload)
    if "discord.com/api/webhooks" in url or "discordapp.com/api/webhooks" in url:
        body = {"content": text}
    elif "api.telegram.org" in url:
        body = {"chat_id": cfg["telegram_chat_id"], "text": text}
    else:  # n8n, Make, Home Assistant, endpoint propio...
        body = {**payload, "message": text}
    try:
        resp = requests.post(url, json=body, timeout=15)
        resp.raise_for_status()
        log.info("Webhook enviado (HTTP %s)", resp.status_code)
    except requests.RequestException as exc:
        log.error("Error enviando webhook: %s", exc)


# --------------------------------------------------------------------------- #
# Trabajo diario
# --------------------------------------------------------------------------- #
def flight_to_dict(f, price, currency):
    return {
        "airline": f.name, "price": price, "currency": currency,
        "departure": f.departure, "arrival": f.arrival,
        "arrival_time_ahead": f.arrival_time_ahead or "", "duration": f.duration,
        "stops": f.stops if isinstance(f.stops, int) else None,
        "stops_text": describe_stops(f.stops), "is_best": f.is_best, "delay": f.delay,
    }


def search(cfg):
    """Consulta Google Flights y devuelve (price_level, modo, [(Flight, precio)]) ordenado por precio.

    Lanza RuntimeError si no se obtiene ningún vuelo con precio.
    """
    result, mode = fetch_flights(cfg)
    priced = [(f, parse_price(f.price)) for f in result.flights]
    priced = [(f, p) for f, p in priced if p]
    # Google repite los "mejores vuelos" en la lista general: quitar duplicados
    seen, unique = set(), []
    for f, p in priced:
        key = (f.name, f.departure, f.arrival, p)
        if key not in seen:
            seen.add(key)
            unique.append((f, p))
    if not unique:
        raise RuntimeError("Google devolvió vuelos pero ninguno con precio")
    unique.sort(key=lambda fp: fp[1])
    return result.current_price, mode, unique


def run_check(cfg):
    """Consulta diaria: log, historial y webhook. Devuelve un resumen (dict) o None si falla."""
    try:
        price_level, mode, priced = search(cfg)
    except Exception as exc:  # noqa: BLE001
        log.error("No se pudo obtener precios: %s", exc)
        return None
    best, min_price = priced[0]

    log_flights(cfg, priced, price_level, cfg["top_n"])

    history = [r for r in read_history(cfg["data_file"]) if same_search(r, cfg)]
    prev_prices = [float(r["min_price"]) for r in history if r.get("min_price")]
    previous_min = min(prev_prices) if prev_prices else None
    if prev_prices:
        last = prev_prices[-1]
        diff = min_price - last
        log.info("Respecto a la última consulta: %+.0f %s | mínimo histórico: %.0f %s",
                 diff, cfg["currency"], previous_min, cfg["currency"])

    now = datetime.now().astimezone()
    append_history(cfg["data_file"], {
        "checked_at": now.isoformat(timespec="seconds"),
        "origin": cfg["origin"], "destination": cfg["destination"],
        "departure_date": cfg["departure_date"], "return_date": cfg["return_date"],
        "adults": cfg["adults"], "seat_class": cfg["seat_class"],
        "min_price": f"{min_price:.2f}", "currency": cfg["currency"],
        "price_level": price_level, "airline": best.name,
        "departure": best.departure, "arrival": best.arrival,
        "duration": best.duration, "stops": best.stops, "fetch_mode": mode,
    })
    log.info("Historial actualizado en %s", cfg["data_file"])

    below = cfg["threshold"] is not None and min_price < cfg["threshold"]
    if below:
        log.info("¡PRECIO BAJO UMBRAL! %.0f < %.0f %s", min_price, cfg["threshold"], cfg["currency"])

    summary = {
        "event": "price_alert" if below else "daily_summary",
        "checked_at": now.isoformat(timespec="seconds"),
        "origin": cfg["origin"], "destination": cfg["destination"],
        "departure_date": cfg["departure_date"], "return_date": cfg["return_date"],
        "adults": cfg["adults"], "seat_class": cfg["seat_class"],
        "min_price": min_price, "currency": cfg["currency"],
        "threshold": cfg["threshold"], "below_threshold": below,
        "price_level": price_level, "airline": best.name,
        "departure": best.departure, "arrival": best.arrival,
        "duration": best.duration, "stops": best.stops,
        "stops_text": describe_stops(best.stops), "previous_min": previous_min,
    }
    webhook_sent = bool(cfg["webhook_url"] and (below or cfg["webhook_mode"] == "always"))
    if webhook_sent:
        send_webhook(cfg, summary)
    return {**summary, "fetch_mode": mode, "webhook_sent": webhook_sent}


def setup_logging():
    logging.basicConfig(level=logging.WARNING, format="%(asctime)s %(levelname)-7s %(message)s",
                        datefmt="%Y-%m-%d %H:%M:%S")
    log.setLevel(logging.INFO)  # silencia el log "response: ..." de primp


def main():
    setup_logging()
    try:
        cfg = load_config()
    except ValueError as exc:
        log.error("Configuración inválida: %s", exc)
        return 2

    if cfg["run_mode"] == "once":
        return 0 if run_check(cfg) else 1

    log.info("Planificador interno: consulta diaria a las %s (TZ=%s)",
             cfg["schedule_time"], os.getenv("TZ", "UTC"))
    schedule.every().day.at(cfg["schedule_time"]).do(run_check, cfg)
    if cfg["run_on_start"]:
        run_check(cfg)
    while True:
        schedule.run_pending()
        time.sleep(30)


if __name__ == "__main__":
    sys.exit(main())
