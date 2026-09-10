# Makefile para el proyecto Heat Server

# --- Compilador y Banderas ---
CC = gcc
# -g:      Añade símbolos de depuración.
# -Wall:   Activa todas las advertencias.
# -fPIC:   Genera código independiente de la posición (necesario para .so).
# -pthread: Enlaza con la biblioteca de pthreads.
CFLAGS = -g -Wall
LDFLAGS = -pthread -lm

# --- Nombres de Archivos ---
LIB_NAME = libheat.so
SERVER_EXEC = heat_server

LIB_SOURCES = heat.c
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)

SERVER_SOURCES = server.c socket_utils.c
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)

# --- Reglas de Construcción ---

# Regla por defecto: construye todo.
all: $(LIB_NAME) $(SERVER_EXEC)

# Regla para construir la biblioteca compartida (.so)
$(LIB_NAME): $(LIB_OBJECTS)
	$(CC) -shared -o $@ $^

# Regla para construir el ejecutable del servidor
$(SERVER_EXEC): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -L. -lheat $(LDFLAGS)

# Regla genérica para compilar archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f $(SERVER_EXEC) $(LIB_NAME) *.o