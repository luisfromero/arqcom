"""Alojamiento barato en Google Hotels con Playwright.

Google Hotels no tiene API pública: se abre la página de resultados en Chromium headless y se
leen las fichas. Las fechas, adultos, moneda y orden van codificados en el parámetro `ts`
(un protobuf en base64; los parámetros checkin/checkout de la URL se ignoran), así que se genera
aquí. Orden «Precio más económico»: la primera página trae los ~18 alojamientos más baratos.

Endpoint:
  GET /api/hotels   alojamientos más baratos de una ciudad para unas fechas
                    (de la base de datos si son recientes; si no, búsqueda en vivo ~10 s)
"""

import base64
import logging
import re
import threading
import time
from datetime import date, datetime
from typing import Optional
from urllib.parse import quote

from fastapi import APIRouter, Depends, HTTPException, Query

import flight_tracker as ft

log = logging.getLogger("flights")

CONCURRENCY = int(ft.env("HOTELS_CONCURRENCY", "2"))       # Chromium simultáneos para hoteles
CACHE_TTL = int(ft.env("HOTELS_CACHE_SECONDS", "3600"))
TIMEOUT_MS = int(ft.env("HOTELS_TIMEOUT_MS", "30000"))
MIN_RATING = float(ft.env("HOTELS_MIN_RATING", "3.5"))     # filtros por defecto de la API
MIN_REVIEWS = int(ft.env("HOTELS_MIN_REVIEWS", "20"))
GOOGLE_HOTELS = "https://www.google.com/travel/search"

# Nombres que suelen indicar cama en habitación compartida
HOSTEL_RE = re.compile(r"hostel|albergue|auberge|jugendherberge|youth|backpacker|"
                       r"pousada de juventude|ostello|хостел|hostell|schronisko|"
                       r"generator |wombat|st christopher|a&o |meininger|clink", re.I)

_sem = threading.BoundedSemaphore(CONCURRENCY)
_cache = {}
_cache_lock = threading.Lock()


# --------------------------------------------------------------------------- #
# Parámetro ts (protobuf mínimo)
# --------------------------------------------------------------------------- #
def _varint(n):
    out = bytearray()
    while True:
        b, n = n & 0x7F, n >> 7
        out.append(b | 0x80 if n else b)
        if not n:
            return bytes(out)


def _int(field, value):
    return _varint(field << 3) + _varint(value)


def _msg(field, payload):
    return _varint(field << 3 | 2) + _varint(len(payload)) + payload


def _date(d):
    return _int(1, d.year) + _int(2, d.month) + _int(3, d.day)


def build_ts(checkin, checkout, adults=1, currency="EUR", cheapest_first=True):
    """ts = {1:1, 2:{1:{1:3}×adultos}, 3:{2:{2:{1:entrada, 2:salida, 3:noches}}}, 5:{1:{5:orden, 7:moneda}}}"""
    guests = b"".join(_msg(1, _int(1, 3)) for _ in range(adults)) + _int(2, 0)
    stay = _msg(1, _date(checkin)) + _msg(2, _date(checkout)) + _int(3, (checkout - checkin).days)
    options = (_int(5, 3) if cheapest_first else b"") + _msg(7, currency.encode())
    raw = (_int(1, 1) + _msg(2, guests) + _msg(3, _msg(2, _msg(2, stay) + _msg(6, _int(2, 0))))
           + _msg(5, _msg(1, options)))
    return base64.urlsafe_b64encode(raw).decode().rstrip("=")


def hotels_url(city, checkin, checkout, adults=1, currency="EUR"):
    ts = build_ts(checkin, checkout, adults, currency)
    return f"{GOOGLE_HOTELS}?q={quote('hoteles en ' + city)}&ts={ts}&hl=es&gl=es&curr={currency}"


# --------------------------------------------------------------------------- #
# Extracción
# --------------------------------------------------------------------------- #
# Una ficha = el mayor antepasado del enlace «Precios de …» que contiene un solo <h2>
EXTRACT_JS = r"""
() => {
  const out = [], seen = new Set();
  for (const a of document.querySelectorAll('a[aria-label^="Precios de "]')) {
    let card = a;
    while (card.parentElement && card.parentElement.querySelectorAll('h2').length <= 1)
      card = card.parentElement;
    if (seen.has(card)) continue;
    seen.add(card);
    const h2 = card.querySelector('h2');
    const rating = card.querySelector('[role="img"][aria-label*=" de 5"]');
    const ent = card.querySelector('[data-href*="/entity/"], a[href*="/entity/"]');
    const entityRef = ent ? (ent.getAttribute('data-href') || ent.getAttribute('href')) : null;
    out.push({
      name: h2 ? h2.textContent.trim() : null,
      label: a.getAttribute('aria-label'),
      rating: rating ? rating.getAttribute('aria-label') : null,
      entity: entityRef,
      text: card.textContent,
    });
  }
  return out;
}
"""


def _num(text):
    """'1.234' -> 1234.0 ; '4,2' -> 4.2"""
    return float(text.replace(".", "").replace(",", ".").replace(" ", ""))


def parse_cards(raw, nights, currency, ts=None):
    hotels, seen = [], set()
    for c in raw:
        name = c.get("name")
        m = re.search(r"a partir de\D*([\d.]+)", c.get("label") or "")
        if not name or not m:
            continue
        night = _num(m.group(1))
        t = re.search(r"([\d.]+)\s*\S{0,3}\s*en total", c.get("text") or "")
        total = _num(t.group(1)) if t else night * nights
        r = re.match(r"([\d,]+) de 5.*?de ([\d.]+)", c.get("rating") or "")
        stars = re.search(r"Hotel de (\d) estrella", c.get("text") or "")
        hid = re.search(r"/entity/([^/?]+)", c.get("entity") or "")
        key = hid.group(1) if hid else name
        if key in seen:
            continue
        seen.add(key)
        hotels.append({
            "hotel_id": key, "name": name,
            "price_total": total, "price_night": round(total / nights, 2) if nights else night,
            "currency": currency,
            "rating": _num(r.group(1)) if r else None,
            "reviews": int(_num(r.group(2))) if r else None,
            "stars": int(stars.group(1)) if stars else None,
            "hostel": bool(HOSTEL_RE.search(name)),
            # Comparativa de precios del alojamiento con las mismas fechas
            "url": (f"https://www.google.com/travel/hotels/entity/{hid.group(1)}/prices"
                    f"?ts={ts}&hl=es&gl=es&curr={currency}") if hid and ts else None,
        })
    hotels.sort(key=lambda h: (h["price_total"], -(h["rating"] or 0)))
    return hotels


def _fetch(url):
    from playwright.sync_api import TimeoutError as PwTimeout
    from playwright.sync_api import sync_playwright

    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        try:
            ctx = browser.new_context(locale="es-ES", user_agent=ft.CHROME_UA)
            ctx.add_cookies([{"name": k, "value": v, "domain": ".google.com", "path": "/"}
                             for k, v in ft.consent_cookies().items()])
            page = ctx.new_page()
            page.goto(url, wait_until="domcontentloaded", timeout=TIMEOUT_MS)
            try:
                page.wait_for_selector('a[aria-label^="Precios de "]', timeout=TIMEOUT_MS)
            except PwTimeout:
                return []          # sin alojamientos para esas fechas
            page.wait_for_timeout(800)  # deja terminar de pintar las fichas
            return page.evaluate(EXTRACT_JS)
        finally:
            browser.close()


def search_hotels(city, checkin, checkout, adults=1, currency="EUR", refresh=False, retries=1):
    """Alojamientos ordenados por precio total. Devuelve (url, hotels, cached)."""
    nights = (checkout - checkin).days
    if nights < 1:
        raise ValueError("checkout debe ser posterior a checkin")
    url = hotels_url(city, checkin, checkout, adults, currency)
    key = (city.lower(), checkin, checkout, adults, currency)
    now = time.time()
    with _cache_lock:
        hit = _cache.get(key)
        if hit and not refresh and now - hit[0] < CACHE_TTL:
            return url, hit[1], True
        for k in [k for k, v in _cache.items() if now - v[0] >= CACHE_TTL]:
            del _cache[k]
    last_exc = None
    for attempt in range(retries + 1):
        try:
            with _sem:
                raw = _fetch(url)
            hotels = parse_cards(raw, nights, currency, build_ts(checkin, checkout, adults, currency))
            with _cache_lock:
                _cache[key] = (time.time(), hotels)
            return url, hotels, False
        except Exception as exc:  # noqa: BLE001
            last_exc = exc
            log.debug("Hoteles %s %s/%s intento %d: %s", city, checkin, checkout, attempt + 1, exc)
            time.sleep(2 * (attempt + 1))
    raise RuntimeError(f"Google Hotels no respondió: {last_exc}")


def filter_hotels(hotels, min_rating=0.0, min_reviews=0, max_night=None, include_hostels=True):
    out = []
    for h in hotels:
        if min_rating and (h["rating"] or 0) < min_rating:
            continue
        if min_reviews and (h["reviews"] or 0) < min_reviews:
            continue
        if max_night is not None and h["price_night"] > max_night:
            continue
        if not include_hostels and h["hostel"]:
            continue
        out.append(h)
    return out


# --------------------------------------------------------------------------- #
# API
# --------------------------------------------------------------------------- #
def make_router(require_key, db):
    """db: módulo precompute (lectura/escritura de la tabla hotels)."""
    from deals import CAPITALS

    router = APIRouter(prefix="/api/hotels", tags=["hoteles"], dependencies=[Depends(require_key)])

    @router.get("")
    def hotels(
        destination: str = Query(..., description="Código de capital (LIS, LON…) o nombre de ciudad"),
        checkin: date = Query(..., description="Fecha de entrada (normalmente la de llegada del vuelo)"),
        checkout: date = Query(..., description="Fecha de salida"),
        adults: int = Query(1, ge=1, le=6),
        min_rating: float = Query(MIN_RATING, ge=0, le=5),
        min_reviews: int = Query(MIN_REVIEWS, ge=0),
        max_price_night: Optional[float] = Query(None, gt=0),
        include_hostels: bool = Query(True, description="Incluir albergues (cama en habitación compartida)"),
        limit: int = Query(10, ge=1, le=50),
        refresh: bool = Query(False, description="Ignorar lo guardado y buscar en vivo (~10 s)"),
    ):
        """Alojamientos más baratos. Usa lo precalculado si es reciente; si no, busca en vivo."""
        if checkin < date.today():
            raise HTTPException(status_code=422, detail=["checkin no puede estar en el pasado"])
        if checkout <= checkin or (checkout - checkin).days > 30:
            raise HTTPException(status_code=422, detail=["checkout debe ser 1-30 noches después de checkin"])
        code = destination.upper()
        city = CAPITALS[code][0] if code in CAPITALS else destination
        nights = (checkout - checkin).days

        source, checked_at, url, cached = "live", None, None, False
        stored = None if (refresh or adults != 1) else db.stored_hotels(code, checkin, nights)
        if stored:
            source, checked_at, url, found = "precomputed", stored["checked_at"], stored["url"], stored["hotels"]
        else:
            try:
                url, found, cached = search_hotels(city, checkin, checkout, adults, refresh=refresh)
            except RuntimeError as exc:
                raise HTTPException(status_code=502, detail=str(exc))
            if adults == 1 and code in CAPITALS:
                db.save_hotels(code, checkin, checkout, url, found)
        result = filter_hotels(found, min_rating, min_reviews, max_price_night, include_hostels)
        return {
            "destination": code if code in CAPITALS else None, "city": city,
            "checkin": checkin.isoformat(), "checkout": checkout.isoformat(), "nights": nights,
            "adults": adults, "source": source, "cached": cached, "checked_at": checked_at,
            "google_hotels_url": url,
            "filters": {"min_rating": min_rating, "min_reviews": min_reviews,
                        "max_price_night": max_price_night, "include_hostels": include_hostels},
            "found": len(found), "count": len(result), "hotels": result[:limit],
        }

    @router.post("/scan", status_code=202)
    def scan_hotels():
        """Busca ya alojamiento para las ofertas de vuelo sin datos recientes (sin esperar al escaneo)."""
        if not db.HOTELS_ENABLED:
            raise HTTPException(status_code=404, detail="Alojamiento desactivado (HOTELS_ENABLED=false)")
        if not db.start_hotels_thread():
            raise HTTPException(status_code=409, detail="Ya hay un precálculo en curso")
        return {"status": "started", "progress_url": "/api/deals/scans"}

    return router
