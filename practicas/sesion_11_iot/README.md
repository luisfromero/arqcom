# Práctica IoT (1 hora): Mosquitto + n8n + Ecowitt + AEMET + Shelly

Esta práctica guía a tus alumnos para montar, en 1 hora, una infraestructura IoT con:

- Broker MQTT (Mosquitto)
- n8n (automatización/no-code)
- Publicación de datos meteo desde estación Ecowitt (vía `ecowitt2mqtt`) o simulados
- Consulta de lluvia a AEMET (o alternativa Open‑Meteo)
- Control de un dispositivo Shelly (encender/apagar) vía MQTT
- Visualización con MQTT Explorer

Imágenes de referencia:

- Estación Ecowitt (ejemplo GW2000):
  ![Ecowitt](https://static.ecowitt.net/upload/product/20220513/1652422756491457.png)
- Shelly 3EM Mini:
  ![Shelly 3EM Mini](https://shelly-website.s3.eu-central-1.amazonaws.com/media/products/3em-mini/3EM-Mini.png)

Nota: Si no tenéis físicamente la estación o el Shelly, se pueden simular mensajes MQTT para completar la práctica.

---

## 0. Requisitos
- Docker y Docker Compose instalados
- MQTT Explorer instalado en vuestro equipo
- Clave de API de AEMET (opcional, recomendable). Alternativa sin clave: Open‑Meteo
- (Opcional) Dispositivo Shelly con soporte MQTT (por ejemplo, Shelly 1/1PM Gen1) o cualquier Shelly compatible

---

## 1. Puesta en marcha de la infraestructura
En `sesion_11_iot/` ya tienes preparado el `docker-compose.yml` y la configuración de Mosquitto.

1) Copia el `env.example` de n8n a `.env` y edítalo si quieres usar OpenAI o variables personalizadas:
```bash
cd sesion_11_iot
copy n8n\env.example n8n\.env   # Windows PowerShell: cp n8n\env.example n8n\.env
```

2) Levanta los servicios:
```bash
docker compose up -d
```
- Mosquitto → `mqtt://localhost:1883` (WebSockets opcional: `ws://localhost:9001`)
- n8n → `http://localhost:5678`
- ecowitt2mqtt → espera datos Ecowitt por `http://<tu_ip_local>:8080`

3) Verifica contenedores:
```bash
docker compose ps
```

---

## 2. Configurar Ecowitt (opcional real; si no, simular)
Si disponéis de una pasarela Ecowitt (p. ej., GW2000):

- En la app/web de Ecowitt, sección Custom/Customized Server:
  - Server: IP de tu PC (en la misma red que la pasarela)
  - Port: `8080`
  - Path: `/` (por defecto)
  - Protocol: HTTP
- Guarda. La pasarela enviará datos periódicamente hacia `ecowitt2mqtt` y este los publicará en MQTT bajo temas tipo `ecowitt/<sensor>/...`.

Si NO tenéis estación, simulad mensajes desde MQTT Explorer publicando en un tema de prácticas, p. ej.: `ecowitt/sim/lluvia` con payloads simples (`0`, `1`, `{"rain_mm": 0.0}`), según lo que pidan los nodos de n8n.

---

## 3. Configurar Shelly para MQTT
Si tenéis un Shelly Gen1 (Shelly 1/1PM, etc.):
- En el panel web del Shelly → Internet & Security → Advanced → habilitar MQTT
- Server: `tcp://<IP_PC>:1883`
- Usuario/contraseña: (en esta práctica, Mosquitto permite anónimo; podéis dejar en blanco)
- Guardar y reiniciar si lo pide

Temas típicos (Gen1):
- Comando ON/OFF del relé 0: `shellies/<device_id>/relay/0/command` con payload `on` o `off`
- Estado del relé: `shellies/<device_id>/relay/0`

Si tenéis un Shelly diferente (Plus/Gen2), los temas pueden variar (MQTT nativo en Gen2 requiere configuración adicional o scripts). Para la práctica, también podéis simular un Shelly publicando/subs.

---

## 4. MQTT Explorer
- Conecta a `localhost:1883`
- Observa temas `ecowitt/#` y los del Shelly
- Prueba a publicar manualmente en `shellies/<device_id>/relay/0/command` → `on` y `off`

---

## 5. n8n: flujo “¿Llueve?” con decisión y acción
Opciones:
- Importar el flujo de ejemplo incluido: `n8n/workflows/lluvia_iot_ejemplo.json`
- O crearlo a mano (recomendado para aprender):

Pasos sugeridos (creación manual):
1) Nodo MQTT Trigger: suscríbete a `ecowitt/#` (o `ecowitt/sim/lluvia` si simulas)
2) Nodo HTTP Request: consulta AEMET o Open‑Meteo:
   - AEMET (requiere API key): `https://opendata.aemet.es/opendata/api/prediccion/especifica/municipio/diaria/<CODMUN>?api_key=<TU_API_KEY>`
   - Open‑Meteo (sin clave): `https://api.open-meteo.com/v1/forecast?latitude=36.72&longitude=-4.42&hourly=precipitation` (Málaga ejemplo)
3) Nodo Function o IF: decide lluvia/posible lluvia a partir de datos (ej., precipitación > 0)
4) Nodo OpenAI (opcional): pásale el contexto (datos de ecowitt + predicción) y que devuelva “llueve/va a llover” y un breve texto.
5) Nodo MQTT Publish: si llueve → publicar `on` en `shellies/<device_id>/relay/0/command`; si no → `off`.
6) Nodo adicional (opcional): enviar notificación o registrar en un Google Sheet.

Sugerencia de lógica simple (sin IA):
- Si `precipitación_actual > 0` o `prob_precipitacion > 50%` → `on` → Shelly ON
- En caso contrario → `off` → Shelly OFF

---

## 6. Importar el flujo de ejemplo
1) Abre n8n → Workflows → Import → elige `n8n/workflows/lluvia_iot_ejemplo.json`
2) Edita los parámetros: tema MQTT, coordenadas, AEMET/Open‑Meteo, `device_id` del Shelly
3) Activa el workflow
4) Genera datos (Ecowitt real o simulados) y observa la automatización

---

## 7. Comandos útiles
```bash
# Subir infraestructura
docker compose up -d

# Ver logs de un servicio
docker compose logs -f mosquitto

# Reiniciar n8n
docker compose restart n8n

# Parar todo
docker compose down
```

---

## 8. Solución de problemas
- n8n no abre: comprueba `http://localhost:5678` y reinicia el contenedor
- No ves mensajes en MQTT Explorer: revisa que está apuntando a `localhost:1883` y que la red Docker permite acceso (en local, sí)
- Ecowitt no publica: revisa IP y puerto `8080` en la configuración del servidor personalizado
- Shelly no responde: revisa los temas correctos, que esté en la misma red y que el broker sea accesible

---

## 9. Créditos y licencias
- Imágenes y productos: Ecowitt y Shelly (enlaces de referencia)
- `ecowitt2mqtt`: `ghcr.io/bachya/ecowitt2mqtt`
- n8n: `https://n8n.io/`

