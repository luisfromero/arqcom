# Práctica IoT: Credenciales para la entrega 2 - Automatización Inteligente con n8n.

## Servidor de envío de correos (UMA) 

**Host:**
correo.uma.es

**Puerto:**
587

**Usuario:**
felipe@uma.es

**Password:**
**********

**SSL**
No

**Disable StartTLS**
NO

**Client Host Name**
Vacío

## Servidor de envío de correos (GMAIL) 

**Host:**
smtp.gmail.es

**Puerto:**
465

**Usuario:**
tunombre@gmail.com

**Password:**
Acceder a https://myaccount.google.com/apppasswords y crear un password. Puedes usar como nombre n8n-smtp. Te genera un password único.


**SSL**
Si

**Client Host Name**
Vacío


## Acceso a la API de Aemet:

**Tipo de credencial:**
Header Auth

**Nombre:**
api_key

**Enlace para obtener la clave:**
https://opendata.aemet.es/centrodedescargas/altaUsuario


# Acceso a Mosquitto

**Tipo de credencial:**
MQTT

**Protocolo:**
mqtts

**Host:**
casium.uma.es

**Puerto:**
8883

**Usuario:**
practicas

**Password:**
labdac

**Otros:**
Activar *clean session* y *ssl*


# Acceso a base de datos:

**Tipo de credencial:**
MySQL

**Host:**
casium.uma.es

*Database:*
mi_database

**Usuario:**
mi_user

**Password:**
mi_password

**Puerto:**
8090

