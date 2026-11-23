# python:3.11-slim fijado a Debian bookworm: "playwright install --with-deps"
# conoce los paquetes de sistema de bookworm (en trixie algunos cambiaron de nombre).
FROM python:3.11-slim-bookworm

ARG INSTALL_PLAYWRIGHT=true

ENV PYTHONUNBUFFERED=1 \
    PYTHONDONTWRITEBYTECODE=1 \
    PIP_NO_CACHE_DIR=1 \
    PLAYWRIGHT_BROWSERS_PATH=/ms-playwright \
    TZ=Europe/Madrid

WORKDIR /app

RUN apt-get update \
    && apt-get install -y --no-install-recommends tzdata ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install -r requirements.txt

# Chromium + dependencias de sistema para el modo "local" (renderizado JS).
# Para una imagen mínima sin navegador: docker compose build --build-arg INSTALL_PLAYWRIGHT=false
RUN if [ "$INSTALL_PLAYWRIGHT" = "true" ]; then \
        playwright install --with-deps chromium && rm -rf /var/lib/apt/lists/*; \
    fi

COPY flight_tracker.py api.py deals.py precompute.py hotels.py ./

VOLUME ["/data"]
EXPOSE 8000

# API REST + seguimiento diario. Para una consulta puntual (cron):
#   docker compose run --rm -e RUN_MODE=once flights python flight_tracker.py
CMD ["python", "api.py"]
