"""API REST de precios de vuelos (para la app Android).

Arranque:  python api.py   (o: uvicorn api:app --host 0.0.0.0 --port 8000)
Docs interactivas: http://<host>:8000/docs
"""

import logging
import os
import secrets
import threading
import time
from contextlib import asynccontextmanager
from typing import Literal, Optional

import schedule
import uvicorn
from fastapi import Depends, FastAPI, HTTPException, Query, Security
from fastapi.security import APIKeyHeader

import flight_tracker as ft
import precompute
from deals import make_router

log = logging.getLogger("flights")

API_KEY = ft.env("API_KEY")
CACHE_TTL = int(ft.env("CACHE_TTL_SECONDS", "900"))
# Prefijo cuando la API va detrás de un proxy inverso que lo elimina (p. ej. "/vuelos-api").
# Solo afecta a /docs y /openapi.json; las rutas siguen siendo /api/... dentro del contenedor.
ROOT_PATH = ft.env("ROOT_PATH").rstrip("/")
MAX_RESULTS = 50

ft.setup_logging()
cfg = ft.load_config(require_route=False)
_cache = {}                      # clave de búsqueda -> (instante, respuesta)
_cache_lock = threading.Lock()
_last_check = {"result": None}


def cached_search(s, refresh=False):
    """Búsqueda con caché de CACHE_TTL segundos. Devuelve (datos, venía_de_caché).

    Lanza la excepción de ft.search si Google no devuelve precios.
    """
    key = tuple(sorted(s.items()))
    with _cache_lock:
        hit = _cache.get(key)
    if hit and not refresh and time.time() - hit[0] < CACHE_TTL:
        return hit[1], True

    price_level, mode, priced = ft.search(s)
    data = {
        "query": {k: v for k, v in s.items() if k != "fetch_mode"},
        "checked_at": ft.datetime.now().astimezone().isoformat(timespec="seconds"),
        "price_level": price_level, "fetch_mode": mode,
        "min_price": priced[0][1], "currency": s["currency"],
        "total_results": len(priced),
        "google_flights_url": ft.google_flights_url(s),
        "flights": [ft.flight_to_dict(f, p, s["currency"]) for f, p in priced[:MAX_RESULTS]],
    }
    with _cache_lock:
        now = time.time()
        for k in [k for k, (t, _) in _cache.items() if now - t >= CACHE_TTL]:
            del _cache[k]           # purga para que la caché no crezca sin límite
        _cache[key] = (now, data)
    return data, False


# --------------------------------------------------------------------------- #
# Seguridad: clave en la cabecera X-API-Key (si API_KEY está definida)
# --------------------------------------------------------------------------- #
api_key_header = APIKeyHeader(name="X-API-Key", auto_error=False)


def require_key(key: Optional[str] = Security(api_key_header)):
    if API_KEY and not (key and secrets.compare_digest(key, API_KEY)):
        raise HTTPException(status_code=401, detail="X-API-Key ausente o incorrecta")


# --------------------------------------------------------------------------- #
# Planificador diario en segundo plano
# --------------------------------------------------------------------------- #
def tracked_check():
    _last_check["result"] = ft.run_check(cfg)
    return _last_check["result"]


def scheduler_loop():
    # Precálculo de ofertas (escaneos programados en hilos propios: no bloquean este bucle)
    precompute.setup(schedule, cached_search)
    if cfg["tracking"]:
        schedule.every().day.at(cfg["schedule_time"]).do(tracked_check)
        if cfg["run_on_start"]:
            tracked_check()
    while True:
        schedule.run_pending()
        time.sleep(30)


@asynccontextmanager
async def lifespan(_app):
    if not API_KEY:
        log.warning("API_KEY vacía: la API queda abierta a cualquiera que llegue al puerto")
    if precompute.ENABLED:
        precompute.init_db()
    if cfg["tracking"]:
        log.info("Seguimiento diario de %s->%s a las %s (TZ=%s)", cfg["origin"],
                 cfg["destination"], cfg["schedule_time"], os.getenv("TZ", "UTC"))
    else:
        log.info("Sin ORIGIN/DESTINATION: solo búsquedas bajo demanda, sin seguimiento diario")
    threading.Thread(target=scheduler_loop, daemon=True, name="scheduler").start()
    yield


app = FastAPI(title="Flight Tracker API", version="1.2", lifespan=lifespan, root_path=ROOT_PATH)
# El router de precálculo va primero: /api/deals/latest y /scans antes que /api/deals/{job_id}
app.include_router(precompute.make_router(cached_search, require_key))
app.include_router(make_router(cached_search, require_key))


# --------------------------------------------------------------------------- #
# Endpoints
# --------------------------------------------------------------------------- #
@app.get("/health")
def health():
    return {"status": "ok", "tracking": cfg["tracking"], "auth": bool(API_KEY)}


@app.get("/api/search", dependencies=[Depends(require_key)])
def search(
    origin: str = Query(..., min_length=3, max_length=3, examples=["AGP"]),
    destination: str = Query(..., min_length=3, max_length=3, examples=["LHR"]),
    departure_date: str = Query(..., examples=["2026-12-15"]),
    return_date: Optional[str] = Query(None, description="Vacío = solo ida"),
    adults: int = 1,
    seat_class: Literal["economy", "premium-economy", "business", "first"] = "economy",
    max_stops: Optional[int] = None,
    currency: str = "EUR",
    limit: int = Query(20, ge=1, le=MAX_RESULTS),
    refresh: bool = Query(False, description="Ignorar la caché"),
):
    """Búsqueda en vivo en Google Flights. Puede tardar varios segundos."""
    s = {
        "origin": origin.upper(), "destination": destination.upper(),
        "departure_date": departure_date, "return_date": return_date or "",
        "adults": adults, "seat_class": seat_class, "max_stops": max_stops,
        "currency": currency.upper(), "fetch_mode": cfg["fetch_mode"],
    }
    errors = ft.validate_search(s)
    if errors:
        raise HTTPException(status_code=422, detail=errors)
    try:
        data, cached = cached_search(s, refresh)
    except Exception as exc:  # noqa: BLE001
        log.error("Búsqueda fallida %s->%s: %s", s["origin"], s["destination"], exc)
        raise HTTPException(status_code=502,
                            detail="No se pudieron obtener precios de Google Flights") from exc
    return {**data, "flights": data["flights"][:limit], "cached": cached}


@app.get("/api/tracker", dependencies=[Depends(require_key)])
def tracker():
    """Ruta en seguimiento diario, umbral y último resultado."""
    if not cfg["tracking"]:
        raise HTTPException(status_code=404, detail="No hay ruta configurada (ORIGIN vacío)")
    return {
        "origin": cfg["origin"], "destination": cfg["destination"],
        "departure_date": cfg["departure_date"], "return_date": cfg["return_date"],
        "adults": cfg["adults"], "seat_class": cfg["seat_class"],
        "max_stops": cfg["max_stops"], "currency": cfg["currency"],
        "threshold": cfg["threshold"], "schedule_time": cfg["schedule_time"],
        "last_check": _last_check["result"],
    }


@app.post("/api/tracker/check", dependencies=[Depends(require_key)])
def tracker_check():
    """Lanza ahora la comprobación de la ruta seguida (guarda historial y envía webhook)."""
    if not cfg["tracking"]:
        raise HTTPException(status_code=404, detail="No hay ruta configurada (ORIGIN vacío)")
    result = tracked_check()
    if result is None:
        raise HTTPException(status_code=502, detail="No se pudieron obtener precios de Google Flights")
    return result


@app.get("/api/history", dependencies=[Depends(require_key)])
def history(
    origin: Optional[str] = None, destination: Optional[str] = None,
    departure_date: Optional[str] = None, return_date: Optional[str] = None,
    limit: int = Query(365, ge=1, le=5000),
):
    """Historial de mínimos diarios. Sin filtros devuelve el de la ruta en seguimiento."""
    filters = {"origin": origin, "destination": destination,
               "departure_date": departure_date, "return_date": return_date}
    if not any(v is not None for v in filters.values()) and cfg["tracking"]:
        filters = {k: cfg[k] for k in filters}
    rows = []
    for r in ft.read_history(cfg["data_file"]):
        if all(v is None or r.get(k, "") == (v.upper() if k in ("origin", "destination") else v)
               for k, v in filters.items()):
            rows.append({**r, "min_price": float(r["min_price"]) if r.get("min_price") else None,
                         "adults": int(r["adults"]) if r.get("adults") else None,
                         "stops": int(r["stops"]) if str(r.get("stops", "")).isdigit() else None})
    rows = rows[-limit:]
    prices = [r["min_price"] for r in rows if r["min_price"] is not None]
    return {
        "filters": filters, "count": len(rows),
        "lowest": min(prices) if prices else None,
        "highest": max(prices) if prices else None,
        "records": rows,
    }


if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=int(ft.env("PORT", "8000")))
