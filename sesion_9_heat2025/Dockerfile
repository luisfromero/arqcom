# prompt para generar este Dockerfile con IA:
# "Necesito un Dockerfile basado en Ubuntu 22.04 para desarrollo. Debe incluir:
# 1. Compiladores gcc y g++.
# 2. Herramientas de compilación cruzada para RISC-V (gcc-riscv64-linux-gnu).
# 3. Python 3 con pip, FastAPI y Uvicorn.
# 4. Herramientas básicas de consola de Linux para principiantes (nano, man, tree, less).
# 5. Un directorio de trabajo en /app.
# 6. Exponer el puerto 8000.
# 7. Un comando por defecto para mantener el contenedor activo.
# 8. Comentarios explicando cómo usarlo."

# Usa una imagen base de Ubuntu 22.04, una LTS estable y conocida.
FROM ubuntu:22.04

# Etiqueta para identificar al autor de la imagen.
LABEL maintainer="felipe"

# Evita que las instalaciones de paquetes pidan confirmación interactiva.
ENV DEBIAN_FRONTEND=noninteractive

# Actualiza la lista de paquetes e instala todas las herramientas necesarias en una sola capa.
# Esto optimiza el tamaño de la imagen.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      # Compiladores C/C++ para la arquitectura del host
      build-essential \
      gdb \
      # Cadena de herramientas para compilación cruzada a RISC-V
      gcc-riscv64-linux-gnu \
      g++-riscv64-linux-gnu \
      gdb-multiarch \
      # Entorno de Python
      python3 \
      python3-pip \
      python3-venv \
      # Herramientas básicas de consola para principiantes
      nano \
      man-db \
      tree \
      less \
      # Servidor SSH para acceso remoto
      openssh-server \
    # Y finalmente, limpia la caché de apt para reducir el tamaño de la imagen.
    && rm -rf /var/lib/apt/lists/*

# --- Configuración de SSH ---
# 1. Permite el login del usuario root.
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
# 2. Establece una contraseña para root. ¡CAMBIA ESTA CONTRASEÑA!
RUN echo 'root:password' | chpasswd

# Reemplaza /bin/sh con un enlace a /bin/bash.
# Esto fuerza a que el shell por defecto sea bash, incluso para herramientas como Docker Desktop.
RUN rm /bin/sh && ln -s /bin/bash /bin/sh

# Instala las librerías de Python necesarias para el servicio web.
RUN pip3 install fastapi uvicorn

# Crea y define el directorio de trabajo. Los comandos posteriores se ejecutarán aquí.
WORKDIR /app

# Expone el puerto 8000 para que la API de FastAPI sea accesible desde el host.
# También exponemos el puerto 22 para SSH y el rango para los servidores de prácticas.

# Puerto FastAPI
EXPOSE 8000 
# Puerto ssh (acceso seguro terminal shell remoto)
EXPOSE 22
# Puerto prácticas
EXPOSE 8010-8020

# Comando por defecto que mantiene el contenedor en ejecución de forma indefinida.
# Es útil para un entorno de desarrollo al que te conectarás repetidamente.
# Modificamos el CMD para que inicie el servicio SSH y luego mantenga el contenedor vivo.
CMD service ssh start && tail -f /dev/null


# --- Cómo usar este Dockerfile ---
#
# 1. Guarda este archivo como "Dockerfile" en la carpeta raíz de tu proyecto.
#
# 2. Construye la imagen de Docker:
#    (Reemplaza 'arqcom_image' con el nombre que prefieras para la imagen)
#    docker build -t arqcom_image .
#
# 3. Ejecuta el contenedor:
#    (Esto mapea tu carpeta actual al directorio /app del contenedor y expone el puerto 8000)
#    
#docker run -d -v "$(pwd)":/app -p 8000:8000 --name arqcom_container arqcom_image
# 4. Accede a la terminal del contenedor para trabajar:
#    docker exec -it arqcom_container /bin/bash
#
# 5. Dentro del contenedor, ya puedes compilar:
#    # Para la arquitectura nativa (x86_64)
#    gcc mi_programa.c -o mi_programa_x86
#
#    # Para RISC-V (compilación cruzada)
#    riscv64-linux-gnu-gcc mi_programa.c -o mi_programa_riscv
#
#    # Para ejecutar tu API de FastAPI
#    uvicorn main:app --host 0.0.0.0 --port 8000 --reload