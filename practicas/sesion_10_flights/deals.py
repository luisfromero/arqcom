"""Ofertas de ida y vuelta a capitales europeas con fechas abiertas.

Google Flights no ofrece (vía fast-flights) el calendario completo de precios, así que se
muestrean combinaciones (destino × fecha de salida × noches) y se lanzan en paralelo como un
trabajo en segundo plano: POST /api/deals crea el trabajo y GET /api/deals/{id} da el progreso.
"""

import logging
import re
import threading
import time
import uuid
from concurrent.futures import ThreadPoolExecutor
from datetime import date, timedelta
from typing import List, Literal, Optional

from fastapi import APIRouter, Depends, HTTPException
from pydantic import BaseModel, Field, field_validator, model_validator

import flight_tracker as ft

log = logging.getLogger("flights")

# Códigos de ciudad (LON, PAR, ROM, STO, BUH…) = todos los aeropuertos de la ciudad,
# incluidos los de low-cost (Stansted, Beauvais, Ciampino, Skavsta…).
# Comprobado: LON, PAR, ROM y STO funcionan; BUH no (se usa OTP).
CAPITALS = {
    "LIS": ("Lisboa", "Portugal"), "MAD": ("Madrid", "España"),
    "PAR": ("París", "Francia"), "LON": ("Londres", "Reino Unido"),
    "DUB": ("Dublín", "Irlanda"), "BRU": ("Bruselas", "Bélgica"),
    "AMS": ("Ámsterdam", "Países Bajos"), "LUX": ("Luxemburgo", "Luxemburgo"),
    "BER": ("Berlín", "Alemania"), "CPH": ("Copenhague", "Dinamarca"),
    "OSL": ("Oslo", "Noruega"), "STO": ("Estocolmo", "Suecia"),
    "HEL": ("Helsinki", "Finlandia"), "KEF": ("Reikiavik", "Islandia"),
    "ROM": ("Roma", "Italia"), "VIE": ("Viena", "Austria"),
    "PRG": ("Praga", "Chequia"), "WAW": ("Varsovia", "Polonia"),
    "BUD": ("Budapest", "Hungría"), "BTS": ("Bratislava", "Eslovaquia"),
    "LJU": ("Liubliana", "Eslovenia"), "ZAG": ("Zagreb", "Croacia"),
    "BEG": ("Belgrado", "Serbia"), "SJJ": ("Sarajevo", "Bosnia y Herzegovina"),
    "TGD": ("Podgorica", "Montenegro"), "TIA": ("Tirana", "Albania"),
    "SKP": ("Skopie", "Macedonia del Norte"), "ATH": ("Atenas", "Grecia"),
    "SOF": ("Sofía", "Bulgaria"), "OTP": ("Bucarest", "Rumanía"),
    "RMO": ("Chisináu", "Moldavia"), "MLA": ("La Valeta", "Malta"),
    "RIX": ("Riga", "Letonia"), "VNO": ("Vilna", "Lituania"),
    "TLL": ("Tallin", "Estonia"),
}

DEFAULT_ORIGIN = ft.env("DEALS_ORIGIN", "AGP")
WORKERS = int(ft.env("DEALS_WORKERS", "3"))
MAX_QUERIES = int(ft.env("DEALS_MAX_QUERIES", "400"))
FETCH_MODE = ft.env("DEALS_FETCH_MODE", "common")   # rápido; Playwright haría eterno el escaneo
MAX_RUNNING_JOBS = 2
JOB_TTL = 3600
RETRIES = 2              # Google a veces devuelve la página sin resultados


class DealsRequest(BaseModel):
    # Ejemplo que Swagger (/docs) pone en "Try it out": escapadas de viernes, 2-3 noches, < 100 €.
    # Sin esto, Swagger rellena valores de relleno ("string", hoy, lunes, 0 escalas) que no sirven.
    model_config = {"json_schema_extra": {"examples": [
        {"max_price": 100, "departure_weekdays": [4], "min_nights": 2, "max_nights": 3},
    ]}}

    origin: str = Field(DEFAULT_ORIGIN, description="Aeropuerto de salida (IATA)")
    max_price: float = Field(100, gt=0, description="Precio máximo ida y vuelta por persona")
    date_from: Optional[date] = Field(None, description="Primera fecha de salida (defecto: dentro de 7 días)")
    date_to: Optional[date] = Field(None, description="Última fecha de salida (defecto: date_from + 4 semanas)")
    min_nights: int = Field(2, ge=1, le=30)
    max_nights: int = Field(3, ge=1, le=30)
    departure_weekdays: Optional[List[int]] = Field(
        None, description="Días de salida permitidos, 0=lunes … 6=domingo. Ej.: [4] = viernes")
    step_days: int = Field(7, ge=1, le=60,
                           description="Si no hay departure_weekdays: probar una salida cada N días")
    destinations: Optional[List[str]] = Field(
        None, description="Códigos IATA a probar (defecto: todas las capitales)")
    adults: int = Field(1, ge=1, le=9)
    seat_class: Literal["economy", "premium-economy", "business", "first"] = "economy"
    max_stops: Optional[int] = Field(None, ge=0)
    currency: str = "EUR"

    @field_validator("origin", "currency")
    @classmethod
    def upper(cls, v):
        return v.strip().upper()

    @field_validator("destinations")
    @classmethod
    def upper_list(cls, v):
        if not v:
            return None
        codes = [d.strip().upper() for d in v]
        bad = [c for c in codes if not re.fullmatch(r"[A-Z]{3}", c)]
        if bad:
            raise ValueError(f"destinations: códigos IATA de 3 letras, no válidos: {bad}. "
                             "Omite el campo para probar todas las capitales")
        return codes

    @field_validator("departure_weekdays")
    @classmethod
    def weekdays_range(cls, v):
        if v and any(d < 0 or d > 6 for d in v):
            raise ValueError("departure_weekdays: valores entre 0 (lunes) y 6 (domingo)")
        return v

    @model_validator(mode="after")
    def fill_dates(self):
        today = date.today()
        self.date_from = self.date_from or today + timedelta(days=7)
        # 4 semanas exactas (28 días): siempre 4 salidas de cada día de la semana
        self.date_to = self.date_to or self.date_from + timedelta(days=27)
        if self.date_from < today:
            raise ValueError("date_from no puede estar en el pasado")
        if self.date_to < self.date_from:
            raise ValueError("date_to debe ser posterior a date_from")
        if (self.date_to - self.date_from).days > 180:
            raise ValueError("la ventana de fechas no puede superar 180 días")
        if self.min_nights > self.max_nights:
            raise ValueError("min_nights no puede ser mayor que max_nights")
        return self


def plan_queries(req):
    """Lista de (destino, salida, vuelta) a consultar."""
    dests = req.destinations or [c for c in CAPITALS if c != req.origin]
    departures, d = [], req.date_from
    while d <= req.date_to:
        departures.append(d)
        d += timedelta(days=1)
    if req.departure_weekdays:
        departures = [d for d in departures if d.weekday() in req.departure_weekdays]
    else:
        departures = departures[::req.step_days]
    nights = range(req.min_nights, req.max_nights + 1)
    return [(dest, dep, dep + timedelta(days=n)) for dest in dests for dep in departures for n in nights]


def fetch_combo(cached_search, origin, dest, dep, ret, *, adults=1, seat_class="economy",
                max_stops=None, currency="EUR", refresh=False, cancelled=lambda: False):
    """Consulta una combinación ida y vuelta (con reintentos) y devuelve el resumen de la mejor.

    Lanza excepción si Google no devuelve precios tras RETRIES reintentos.
    """
    s = {"origin": origin, "destination": dest,
         "departure_date": dep.isoformat(), "return_date": ret.isoformat(),
         "adults": adults, "seat_class": seat_class, "max_stops": max_stops,
         "currency": currency, "fetch_mode": FETCH_MODE}
    for attempt in range(RETRIES + 1):
        try:
            data, _ = cached_search(s, refresh)
            break
        except RuntimeError:
            if attempt == RETRIES or cancelled():
                raise
            time.sleep(2 * (attempt + 1))
    best = data["flights"][0]
    return {
        "destination": dest,
        "city": CAPITALS.get(dest, (dest, ""))[0],
        "country": CAPITALS.get(dest, ("", ""))[1],
        "departure_date": s["departure_date"], "return_date": s["return_date"],
        "nights": (ret - dep).days,
        "price": data["min_price"], "currency": currency,
        "price_level": data["price_level"],
        "airline": best["airline"], "departure": best["departure"],
        "arrival": best["arrival"], "duration": best["duration"],
        "stops": best["stops"], "stops_text": best["stops_text"],
        "google_flights_url": data["google_flights_url"],
    }


class Job:
    def __init__(self, req, queries):
        self.id = uuid.uuid4().hex[:12]
        self.req, self.queries = req, queries
        self.created = time.time()
        self.status = "running"
        self.done = 0
        self.errors = 0
        self.results = []          # una entrada por consulta con precio
        self.lock = threading.Lock()
        self.cancelled = False


def make_router(cached_search, require_key):
    router = APIRouter(prefix="/api/deals", tags=["ofertas"], dependencies=[Depends(require_key)])
    jobs = {}

    def run_one(job, dest, dep, ret):
        if job.cancelled:
            return
        r = job.req
        try:
            item = fetch_combo(cached_search, r.origin, dest, dep, ret, adults=r.adults,
                               seat_class=r.seat_class, max_stops=r.max_stops,
                               currency=r.currency, cancelled=lambda: job.cancelled)
            with job.lock:
                job.results.append(item)
        except Exception as exc:  # noqa: BLE001
            log.debug("Ofertas %s->%s %s/%s: %s", r.origin, dest, dep, ret,
                        str(exc).splitlines()[0][:120] if str(exc) else type(exc).__name__)
            with job.lock:
                job.errors += 1
        finally:
            with job.lock:
                job.done += 1

    def run_job(job):
        log.info("Escaneo %s: %d consultas desde %s (máx. %.0f %s)", job.id, len(job.queries),
                 job.req.origin, job.req.max_price, job.req.currency)
        with ThreadPoolExecutor(max_workers=WORKERS) as pool:
            for q in job.queries:
                pool.submit(run_one, job, *q)
        job.status = "cancelled" if job.cancelled else "done"
        log.info("Escaneo %s terminado: %d con precio, %d errores", job.id,
                 len(job.results), job.errors)

    def purge():
        now = time.time()
        for jid in [j for j, job in jobs.items() if now - job.created > JOB_TTL]:
            del jobs[jid]

    def job_view(job, limit):
        with job.lock:
            results = list(job.results)
            done, errors = job.done, job.errors
        max_price = job.req.max_price
        deals = sorted((x for x in results if x["price"] <= max_price), key=lambda x: x["price"])
        # La más barata por destino (ofertas o no), para ver "cuánto cuesta ir" aunque no baje del umbral
        cheapest = {}
        for x in results:
            if x["destination"] not in cheapest or x["price"] < cheapest[x["destination"]]["price"]:
                cheapest[x["destination"]] = x
        best_by_dest = sorted(cheapest.values(), key=lambda x: x["price"])
        tried = list(dict.fromkeys(q[0] for q in job.queries))
        finished = job.status != "running"
        return {
            "job_id": job.id, "status": job.status,
            "progress": {"done": done, "total": len(job.queries), "errors": errors,
                         "percent": round(100 * done / len(job.queries), 1)},
            "request": job.req.model_dump(mode="json"),
            "deals_count": len(deals),
            "deals": deals[:limit],
            "cheapest_by_destination": [
                {k: x[k] for k in ("destination", "city", "country", "price", "currency",
                                   "departure_date", "return_date", "nights", "airline",
                                   "google_flights_url")}
                | {"is_deal": x["price"] <= max_price}
                for x in best_by_dest
            ],
            # Destinos sin ningún precio (ruta inexistente o Google sin datos); solo al terminar
            "no_results": [d for d in tried if d not in cheapest] if finished else [],
        }

    @router.get("/capitals")
    def capitals():
        """Capitales que se prueban por defecto (código de ciudad/aeropuerto, ciudad, país)."""
        return [{"code": c, "city": n, "country": p} for c, (n, p) in CAPITALS.items()]

    @router.post("", status_code=202)
    def start(req: DealsRequest):
        """Lanza un escaneo de ofertas ida y vuelta. Consulta el progreso con GET /api/deals/{job_id}."""
        purge()
        errors = ft.validate_search({"origin": req.origin, "destination": "XXX",
                                     "departure_date": req.date_from.isoformat(),
                                     "adults": req.adults, "seat_class": req.seat_class})
        if errors:
            raise HTTPException(status_code=422, detail=errors)
        queries = plan_queries(req)
        if not queries:
            raise HTTPException(status_code=422,
                                detail=["Ninguna fecha de salida cumple los filtros"])
        if len(queries) > MAX_QUERIES:
            raise HTTPException(status_code=422, detail=[
                f"El escaneo necesita {len(queries)} consultas (máximo {MAX_QUERIES}). "
                "Reduce la ventana de fechas, el rango de noches o los destinos, "
                "o usa departure_weekdays / un step_days mayor."])
        if sum(j.status == "running" and not j.cancelled for j in jobs.values()) >= MAX_RUNNING_JOBS:
            raise HTTPException(status_code=429, detail="Hay demasiados escaneos en curso; espera")
        job = Job(req, queries)
        jobs[job.id] = job
        threading.Thread(target=run_job, args=(job,), daemon=True, name=f"deals-{job.id}").start()
        secs = len(queries) * 1.3 / WORKERS
        return {"job_id": job.id, "status": job.status, "total_queries": len(queries),
                "estimated_seconds": round(secs), "status_url": f"/api/deals/{job.id}"}

    @router.get("/{job_id}")
    def status(job_id: str, limit: int = 100):
        """Progreso y resultados (parciales mientras status = running)."""
        job = jobs.get(job_id)
        if not job:
            raise HTTPException(status_code=404, detail="Trabajo no encontrado o caducado")
        return job_view(job, limit)

    @router.delete("/{job_id}")
    def cancel(job_id: str):
        """Cancela un escaneo en curso (las consultas ya lanzadas terminan)."""
        job = jobs.get(job_id)
        if not job:
            raise HTTPException(status_code=404, detail="Trabajo no encontrado o caducado")
        job.cancelled = True
        return {"job_id": job.id, "status": "cancelling" if job.status == "running" else job.status}

    return router
