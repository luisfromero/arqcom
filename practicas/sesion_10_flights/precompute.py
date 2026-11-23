"""Precálculo periódico de precios en SQLite para que la app obtenga ofertas al instante.

Varias veces al día se consulta una rejilla amplia (capitales × salidas × noches) y se guarda el
precio de CADA combinación, no solo las ofertas. Así cualquier filtro (precio máximo, noches,
días de salida, fechas, destinos) se resuelve con una consulta SQL en milisegundos.

Endpoints (todos bajo /api/deals, registrados ANTES que /api/deals/{job_id}):
  GET  /api/deals/latest   ofertas precalculadas con filtros
  GET  /api/deals/scans    escaneo en curso + últimos escaneos
  POST /api/deals/scans    lanzar un escaneo ahora
"""

import json
import logging
import sqlite3
import threading
import time
from concurrent.futures import ThreadPoolExecutor
from datetime import date, datetime, timedelta
from pathlib import Path
from typing import Optional

from fastapi import APIRouter, Depends, HTTPException, Query

import flight_tracker as ft
from deals import CAPITALS, DEFAULT_ORIGIN, WORKERS, fetch_combo

log = logging.getLogger("flights")


def _int_list(text):
    return [int(x) for x in text.replace(" ", "").split(",") if x != ""]


def _range(text):
    lo, _, hi = text.partition("-")
    return int(lo), int(hi or lo)


ENABLED = ft.env("PRECOMPUTE_ENABLED", "true").lower() in {"1", "true", "yes"}
DB_PATH = Path(ft.env("PRECOMPUTE_DB", "/data/deals.db"))
ORIGIN = ft.env("PRECOMPUTE_ORIGIN", DEFAULT_ORIGIN).upper()
TIMES = [t.strip() for t in ft.env("PRECOMPUTE_TIMES", "06:00,18:00").split(",") if t.strip()]
WEEKDAYS = _int_list(ft.env("PRECOMPUTE_WEEKDAYS", "3,4,5"))        # jue, vie, sáb
MIN_NIGHTS, MAX_NIGHTS = _range(ft.env("PRECOMPUTE_NIGHTS", "2-4"))
WEEKS = int(ft.env("PRECOMPUTE_WEEKS", "8"))
START_DAYS = int(ft.env("PRECOMPUTE_START_DAYS", "2"))            # primera salida: hoy + N
STALE_HOURS = float(ft.env("PRECOMPUTE_STALE_HOURS", "12"))      # al arrancar, escanear si es más viejo
MAX_AGE_HOURS = float(ft.env("PRECOMPUTE_MAX_AGE_HOURS", "36"))  # precios más viejos no se sirven
DAY_NAMES = ["lun", "mar", "mié", "jue", "vie", "sáb", "dom"]

_db_lock = threading.Lock()
_scan_lock = threading.Lock()      # un escaneo cada vez
_current = {"scan": None}          # progreso del escaneo en curso (en memoria)


# --------------------------------------------------------------------------- #
# Base de datos
# --------------------------------------------------------------------------- #
SCHEMA = """
CREATE TABLE IF NOT EXISTS scans (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    origin TEXT, started_at TEXT, finished_at TEXT, status TEXT,
    total INTEGER, done INTEGER, errors INTEGER, found INTEGER, params TEXT
);
CREATE TABLE IF NOT EXISTS prices (
    origin TEXT, destination TEXT, departure_date TEXT, nights INTEGER,
    return_date TEXT, weekday INTEGER,
    price REAL, prev_price REAL, currency TEXT, price_level TEXT,
    airline TEXT, departure TEXT, arrival TEXT, duration TEXT,
    stops INTEGER, stops_text TEXT, google_flights_url TEXT,
    checked_at TEXT, checked_ts INTEGER, scan_id INTEGER,
    PRIMARY KEY (origin, destination, departure_date, nights)
);
CREATE INDEX IF NOT EXISTS idx_prices_origin_price ON prices (origin, price);
"""


def connect():
    conn = sqlite3.connect(DB_PATH, timeout=30)
    conn.row_factory = sqlite3.Row
    return conn


def init_db():
    DB_PATH.parent.mkdir(parents=True, exist_ok=True)
    with _db_lock, connect() as conn:
        conn.execute("PRAGMA journal_mode=WAL")
        conn.executescript(SCHEMA)
        # Escaneos que quedaron a medias por un reinicio del contenedor
        conn.execute("UPDATE scans SET status='interrupted' WHERE status='running'")


def save_price(item, origin, scan_id):
    now = datetime.now().astimezone()
    row = {**item, "origin": origin, "scan_id": scan_id,
           "weekday": date.fromisoformat(item["departure_date"]).weekday(),
           "checked_at": now.isoformat(timespec="seconds"), "checked_ts": int(now.timestamp())}
    cols = ["origin", "destination", "departure_date", "nights", "return_date", "weekday",
            "price", "currency", "price_level", "airline", "departure", "arrival", "duration",
            "stops", "stops_text", "google_flights_url", "checked_at", "checked_ts", "scan_id"]
    updates = ", ".join(f"{c}=excluded.{c}" for c in cols[4:])
    sql = (f"INSERT INTO prices ({', '.join(cols)}) VALUES ({', '.join('?' * len(cols))}) "
           f"ON CONFLICT(origin, destination, departure_date, nights) DO UPDATE SET "
           f"prev_price=prices.price, {updates}")
    with _db_lock, connect() as conn:
        conn.execute(sql, [row[c] for c in cols])


def last_finished_scan():
    with connect() as conn:
        return conn.execute("SELECT * FROM scans WHERE status='done' AND origin=? "
                            "ORDER BY id DESC LIMIT 1", (ORIGIN,)).fetchone()


# --------------------------------------------------------------------------- #
# Escaneo
# --------------------------------------------------------------------------- #
def plan():
    start = date.today() + timedelta(days=START_DAYS)
    end = start + timedelta(weeks=WEEKS) - timedelta(days=1)
    deps, d = [], start
    while d <= end:
        if d.weekday() in WEEKDAYS:
            deps.append(d)
        d += timedelta(days=1)
    dests = [c for c in CAPITALS if c != ORIGIN]
    queries = [(dest, dep, dep + timedelta(days=n))
               for dep in deps for dest in dests for n in range(MIN_NIGHTS, MAX_NIGHTS + 1)]
    params = {"origin": ORIGIN, "date_from": start.isoformat(), "date_to": end.isoformat(),
              "weekdays": WEEKDAYS, "min_nights": MIN_NIGHTS, "max_nights": MAX_NIGHTS,
              "destinations": dests}
    return queries, params


def run_scan(cached_search):
    """Escaneo completo. Devuelve False si ya había otro en curso."""
    if not _scan_lock.acquire(blocking=False):
        return False
    try:
        queries, params = plan()
        started = datetime.now().astimezone().isoformat(timespec="seconds")
        with _db_lock, connect() as conn:
            scan_id = conn.execute(
                "INSERT INTO scans (origin, started_at, status, total, done, errors, found, params) "
                "VALUES (?, ?, 'running', ?, 0, 0, 0, ?)",
                (ORIGIN, started, len(queries), json.dumps(params))).lastrowid
        state = {"id": scan_id, "started_at": started, "total": len(queries),
                 "done": 0, "errors": 0, "found": 0, "params": params}
        _current["scan"] = state
        lock = threading.Lock()
        log.info("Precálculo #%d: %d consultas desde %s (%s→%s)", scan_id, len(queries),
                 ORIGIN, params["date_from"], params["date_to"])
        t0 = time.time()

        def one(dest, dep, ret):
            try:
                # refresh=True: siempre precio fresco (y de paso rellena la caché de /api/deals)
                item = fetch_combo(cached_search, ORIGIN, dest, dep, ret, refresh=True)
                save_price(item, ORIGIN, scan_id)
                with lock:
                    state["found"] += 1
            except Exception as exc:  # noqa: BLE001
                with lock:
                    state["errors"] += 1
                log.debug("Precálculo %s %s/%s: %s", dest, dep, ret, exc)
            finally:
                with lock:
                    state["done"] += 1
                    if state["done"] % 250 == 0:
                        log.info("Precálculo #%d: %d/%d (%d precios, %d sin datos)", scan_id,
                                 state["done"], state["total"], state["found"], state["errors"])

        with ThreadPoolExecutor(max_workers=WORKERS) as pool:
            for q in queries:
                pool.submit(one, *q)

        finished = datetime.now().astimezone().isoformat(timespec="seconds")
        with _db_lock, connect() as conn:
            conn.execute("UPDATE scans SET finished_at=?, status='done', done=?, errors=?, found=? "
                         "WHERE id=?", (finished, state["done"], state["errors"], state["found"], scan_id))
            # Las salidas ya pasadas no sirven: se borran
            conn.execute("DELETE FROM prices WHERE departure_date < ?", (date.today().isoformat(),))
        log.info("Precálculo #%d terminado en %.0f min: %d precios, %d sin datos", scan_id,
                 (time.time() - t0) / 60, state["found"], state["errors"])
        return True
    finally:
        _current["scan"] = None
        _scan_lock.release()


def start_scan_thread(cached_search):
    if _scan_lock.locked():
        return False
    threading.Thread(target=run_scan, args=(cached_search,), daemon=True,
                     name="precompute").start()
    return True


def setup(schedule, cached_search):
    """Llamado desde el hilo planificador de la API."""
    if not ENABLED:
        log.info("Precálculo desactivado (PRECOMPUTE_ENABLED=false)")
        return
    for t in TIMES:
        schedule.every().day.at(t).do(start_scan_thread, cached_search)
    log.info("Precálculo programado a las %s: %s, %d-%d noches, %d semanas, desde %s",
             ", ".join(TIMES), "/".join(DAY_NAMES[d] for d in WEEKDAYS),
             MIN_NIGHTS, MAX_NIGHTS, WEEKS, ORIGIN)
    last = last_finished_scan()
    age_h = None
    if last:
        age_h = (datetime.now().astimezone()
                 - datetime.fromisoformat(last["finished_at"])).total_seconds() / 3600
    if age_h is None or age_h > STALE_HOURS:
        log.info("Sin datos recientes (%s): lanzo un precálculo ahora",
                 "nunca" if age_h is None else f"hace {age_h:.1f} h")
        start_scan_thread(cached_search)


# --------------------------------------------------------------------------- #
# API
# --------------------------------------------------------------------------- #
def _csv(text, cast=str):
    return [cast(x.strip()) for x in text.split(",") if x.strip()] if text else None


def make_router(cached_search, require_key):
    router = APIRouter(prefix="/api/deals", tags=["ofertas precalculadas"],
                       dependencies=[Depends(require_key)])

    @router.get("/latest")
    def latest(
        max_price: float = Query(100, gt=0),
        weekdays: Optional[str] = Query(None, description="Días de salida, ej. '4' o '4,5' (0=lun…6=dom)"),
        min_nights: Optional[int] = Query(None, ge=1),
        max_nights: Optional[int] = Query(None, ge=1),
        date_from: Optional[date] = Query(None, description="Primera fecha de salida"),
        date_to: Optional[date] = Query(None, description="Última fecha de salida"),
        destinations: Optional[str] = Query(None, description="Ej. 'LON,ROM,LIS'"),
        max_stops: Optional[int] = Query(None, ge=0),
        max_age_hours: float = Query(MAX_AGE_HOURS, gt=0, description="Ignorar precios más viejos"),
        limit: int = Query(100, ge=1, le=2000),
        origin: Optional[str] = Query(None, description="Solo se precalcula PRECOMPUTE_ORIGIN"),
    ):
        """Ofertas precalculadas: respuesta instantánea desde la base de datos."""
        if not ENABLED:
            raise HTTPException(status_code=404, detail="Precálculo desactivado")
        try:
            wd = _csv(weekdays, int)
        except ValueError:
            raise HTTPException(status_code=422, detail=["weekdays: números 0-6 separados por comas"])
        dests = [d.upper() for d in _csv(destinations)] if destinations else None

        where = ["origin = ?", "departure_date >= ?", "checked_ts >= ?"]
        # Otro origen distinto del precalculado => sin filas (y coverage_ok=false más abajo)
        args = [(origin or ORIGIN).upper(), date.today().isoformat(),
                int(time.time() - max_age_hours * 3600)]
        if wd:
            where.append(f"weekday IN ({','.join('?' * len(wd))})"); args += wd
        if min_nights:
            where.append("nights >= ?"); args.append(min_nights)
        if max_nights:
            where.append("nights <= ?"); args.append(max_nights)
        if date_from:
            where.append("departure_date >= ?"); args.append(date_from.isoformat())
        if date_to:
            where.append("departure_date <= ?"); args.append(date_to.isoformat())
        if dests:
            where.append(f"destination IN ({','.join('?' * len(dests))})"); args += dests
        if max_stops is not None:
            where.append("stops <= ?"); args.append(max_stops)

        with connect() as conn:
            rows = conn.execute(f"SELECT * FROM prices WHERE {' AND '.join(where)} "
                                "ORDER BY price, departure_date", args).fetchall()
        last = last_finished_scan()

        def item(r):
            d = {k: r[k] for k in ("destination", "departure_date", "return_date", "nights", "price",
                                   "currency", "price_level", "airline", "departure", "arrival",
                                   "duration", "stops", "stops_text", "google_flights_url",
                                   "checked_at", "prev_price")}
            city, country = CAPITALS.get(r["destination"], (r["destination"], ""))
            change = None if r["prev_price"] is None else round(r["price"] - r["prev_price"], 2)
            return {**d, "city": city, "country": country, "price_change": change}

        all_items = [item(r) for r in rows]
        deals = [x for x in all_items if x["price"] <= max_price]
        cheapest = {}
        for x in all_items:                  # ya vienen ordenados por precio
            cheapest.setdefault(x["destination"], x)

        # ¿Cubre el precálculo lo que se pide? Si no, la app debería usar POST /api/deals
        notes = []
        cov = json.loads(last["params"]) if last else None
        if not cov:
            notes.append("Todavía no hay ningún precálculo terminado")
        elif origin and origin.upper() != ORIGIN:
            notes.append(f"Solo se precalculan salidas desde {ORIGIN}")
        else:
            if wd and set(wd) - set(cov["weekdays"]):
                notes.append("Días de salida no precalculados: " + ", ".join(
                    DAY_NAMES[d] for d in sorted(set(wd) - set(cov["weekdays"]))))
            if (min_nights and min_nights < cov["min_nights"]) or (max_nights and max_nights > cov["max_nights"]):
                notes.append(f"Solo se precalculan {cov['min_nights']}-{cov['max_nights']} noches")
            if date_to and date_to.isoformat() > cov["date_to"]:
                notes.append(f"Solo se precalculan salidas hasta {cov['date_to']}")
            if dests and set(dests) - set(cov["destinations"]):
                notes.append("Destinos no precalculados: " + ", ".join(sorted(set(dests) - set(cov["destinations"]))))

        updated_at = last["finished_at"] if last else None
        age_min = (round((datetime.now().astimezone() - datetime.fromisoformat(updated_at))
                         .total_seconds() / 60) if updated_at else None)
        running = _current["scan"]
        return {
            "source": "precomputed",
            "updated_at": updated_at, "age_minutes": age_min,
            "scan_running": ({k: running[k] for k in ("id", "done", "total", "started_at")}
                             if running else None),
            "coverage": ({k: cov[k] for k in ("origin", "date_from", "date_to", "weekdays",
                                              "min_nights", "max_nights")} if cov else None),
            "coverage_ok": not notes,
            "coverage_notes": notes,
            "request": {"origin": ORIGIN, "max_price": max_price, "departure_weekdays": wd,
                        "min_nights": min_nights or (cov or {}).get("min_nights"),
                        "max_nights": max_nights or (cov or {}).get("max_nights"),
                        "date_from": (date_from.isoformat() if date_from else (cov or {}).get("date_from")),
                        "date_to": (date_to.isoformat() if date_to else (cov or {}).get("date_to")),
                        "destinations": dests},
            "deals_count": len(deals),
            "deals": deals[:limit],
            "cheapest_by_destination": [
                {k: x[k] for k in ("destination", "city", "country", "price", "currency",
                                   "departure_date", "return_date", "nights", "airline",
                                   "google_flights_url")} | {"is_deal": x["price"] <= max_price}
                for x in cheapest.values()],
        }

    @router.get("/scans")
    def scans(limit: int = Query(10, ge=1, le=100)):
        """Escaneo de precálculo en curso (con progreso) y los últimos realizados."""
        with connect() as conn:
            recent = [dict(r) for r in conn.execute(
                "SELECT id, origin, started_at, finished_at, status, total, done, errors, found "
                "FROM scans ORDER BY id DESC LIMIT ?", (limit,))]
        running = _current["scan"]
        return {"enabled": ENABLED, "schedule": TIMES,
                "running": ({k: running[k] for k in ("id", "started_at", "total", "done",
                                                     "errors", "found")} if running else None),
                "recent": recent}

    @router.post("/scans", status_code=202)
    def scan_now():
        """Lanza un precálculo ahora (tarda ~30 min con la configuración por defecto)."""
        if not ENABLED:
            raise HTTPException(status_code=404, detail="Precálculo desactivado")
        if not start_scan_thread(cached_search):
            raise HTTPException(status_code=409, detail="Ya hay un precálculo en curso")
        queries, _ = plan()
        return {"status": "started", "total_queries": len(queries),
                "estimated_minutes": round(len(queries) * 2.1 / WORKERS / 60)}  # medido: 2520 en 29 min

    return router
