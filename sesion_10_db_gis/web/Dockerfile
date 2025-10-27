# Usamos una imagen base de PHP-FPM sobre Alpine Linux (ligera y segura)
FROM php:8.4-fpm-alpine

# Instalamos las herramientas necesarias: Nginx, Supervisor y la extensión de PHP para MySQL
RUN apk update && apk add --no-cache \
    nginx \
    supervisor \
    mysql-client

# Instalamos la extensión mysqli de PHP
RUN docker-php-ext-install mysqli pdo pdo_mysql

# Limpiamos la configuración por defecto de Nginx
RUN rm /etc/nginx/http.d/default.conf

# Copiamos nuestros archivos de configuración al contenedor
COPY nginx.conf /etc/nginx/http.d/default.conf
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

# Creamos el directorio para los logs de supervisor
RUN mkdir -p /var/log/supervisor

# Exponemos el puerto 80, que es el que Nginx escuchará dentro del contenedor
EXPOSE 80

# El comando que se ejecutará al iniciar el contenedor.
# 'supervisord' se encargará de lanzar y mantener vivos Nginx y PHP-FPM.
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]
