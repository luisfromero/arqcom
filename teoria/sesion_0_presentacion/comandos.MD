# Línea de comandos en Linux: primeros pasos

## 1. Objetivo de la sesión

En esta primera clase vamos a perder el miedo a la línea de comandos. La terminal no es una "caja negra" ni una herramienta solo para expertos: es la forma más directa de interactuar con un sistema operativo, automatizar tareas, compilar programas, gestionar archivos y conectarse a máquinas remotas.

El objetivo es que el alumnado:

- entienda la estructura del sistema de archivos,
- comprenda qué son los paths y por qué importan,
- conozca los tipos de archivos más comunes,
- aprenda a moverse por un sistema Linux,
- se familiarice con la diferencia entre Windows y Linux,
- acceda remotamente a una máquina Linux desde su equipo local,
- tenga una base sólida para la primera práctica de laboratorio.

---

## 2. Estructura de archivos y directorios

Un sistema operativo organiza la información en una jerarquía de archivos y carpetas. En Linux, esta jerarquía empieza en la raíz `/`.

### Árbol de directorios típico

```bash
/
├── bin/        # programas esenciales
├── boot/       # archivos de arranque
├── dev/        # dispositivos del sistema
├── etc/        # configuración del sistema
├── home/       # directorios personales de usuarios
│   └── alumno/
├── lib/        # librerías del sistema
├── media/      # dispositivos extraíbles
├── opt/        # aplicaciones opcionales
├── proc/       # información del sistema en tiempo real
├── root/       # carpeta del usuario root
├── sbin/       # binarios del sistema
├── tmp/        # archivos temporales
├── usr/        # programas y librerías no esenciales
├── var/        # logs, bases de datos, etc.
└── usr/local/  # software instalado localmente
```

### Directorio personal

Cada usuario tiene un directorio personal, normalmente algo así:

```bash
/home/alumno
/home/usuario
```

Cuando entras en una máquina Linux, normalmente comienzas en tu directorio de trabajo personal:

```bash
pwd
```

Esto te dice la ruta actual en la que estás.

---

## 3. Qué es un path

Un path es la ruta por la que se identifica un archivo o directorio dentro del sistema.

### Tipos de paths

#### 1. Absolute path (ruta absoluta)

Empieza desde la raíz `/`.

```bash
/home/alumno/Documentos
/etc/passwd
/tmp
```

#### 2. Relative path (ruta relativa)

Se interpreta respecto al directorio actual.

```bash
./archivo.txt
../otros/archivo.txt
mi_carpeta/ejemplo.txt
```

### Símbolos clave

- `.` = directorio actual
- `..` = directorio padre
- `/` = raíz del sistema
- `~` = directorio personal del usuario actual

Ejemplo:

```bash
pwd
ls
cd ..
ls
cd ~/Documentos
```

---

## 4. Tipos de archivos y asociaciones

En Linux no todo son archivos "normales" como en Windows. El sistema distingue varios tipos.

### Tipos de ficheros más comunes

#### Archivo regular
Es un fichero normal, con contenido, por ejemplo:

```bash
texto.txt
programa.c
notas.md
```

#### Directorio
Es una carpeta que contiene otros archivos.

```bash
mkdir pruebas
```

#### Enlace simbólico
Es un acceso directo a otro archivo o carpeta.

```bash
ln -s /ruta/origen acceso_rapido
```

#### Socket, pipe, dispositivo
Estos no son archivos normales; pertenecen al sistema y a la gestión del hardware o la comunicación entre procesos.

Ejemplo:

```bash
/dev/sda
/dev/tty
```

### Asociación con tipos

En Linux, muchas veces el contenido importa más que la extensión. Una extensión es útil, pero no siempre tiene significado real para el sistema.

Ejemplo:

```bash
archivo.txt
archivo.md
archivo.c
archivo.py
```

Las extensiones pueden no ser obligatorias, pero ayudan a las personas a saber qué contiene cada fichero.

---

## 5. Linux vs Windows: analogías útiles

Vamos a trabajar en paralelo con Linux y Windows para facilitar la transición.

| Concepto | Windows | Linux |
|---|---|---|
| Raíz | `C:\` | `/` |
| Carpeta personal | `C:\Users\alumno` | `/home/alumno` |
| Directorio actual | `.` | `.` |
| Directorio padre | `..` | `..` |
| Archivo oculto | `archivo.txt` y configuración oculta | archivos que empiezan por `.` como `.bashrc` |
| Ejecutable | `.exe` | sin extensión obligatoria, permisos de ejecución |
| Ruta absoluta | `C:\Users\alumno\Desktop` | `/home/alumno/Desktop` |
| Terminal | PowerShell / CMD | bash / zsh |

### Ejemplo de comparación

En Windows:

```powershell
cd C:\Users\alumno
dir
```

En Linux:

```bash
cd /home/alumno
ls
```

Aunque la sintaxis cambia, la idea es la misma: moverse, listar, crear y editar archivos.

---

## 6. Comandos básicos de navegación

### `pwd`
Muestra el directorio actual.

```bash
pwd
```

### `ls`
Lista el contenido del directorio.

```bash
ls
ls -l
ls -a
ls -lh
```

- `-l`: formato largo
- `-a`: incluye archivos ocultos
- `-h`: legible para humanos

### `cd`
Cambia de directorio.

```bash
cd /home/alumno
cd ..
cd ~
cd ./Documentos
```

### `mkdir`
Crea directorios.

```bash
mkdir practica1
mkdir -p curso/semana1/ejercicios
```

### `touch`
Crea un archivo vacío.

```bash
touch archivo.txt
```

### `cp`
Copia archivos.

```bash
cp archivo.txt copia.txt
cp -r carpeta carpeta_copia
```

### `mv`
Mueve o renombra archivos.

```bash
mv archivo.txt nuevo_nombre.txt
mv archivo.txt /tmp/
```

### `rm`
Elimina archivos o carpetas.

```bash
rm archivo.txt
rm -r carpeta
```

> ¡Cuidado! En Linux, `rm` elimina directamente. Conviene pensarlo siempre antes de pulsar.

---

## 7. Comandos para ver y editar archivos

### `cat`
Muestra el contenido completo de un archivo.

```bash
cat archivo.txt
```

### `less` y `more`
Permiten leer archivos largos página a página.

```bash
less archivo.txt
more archivo.txt
```

### `head` y `tail`
Muestran el principio o el final del fichero.

```bash
head archivo.txt
head -n 20 archivo.txt
tail archivo.txt
```

### `nano` / `vi` / `vim`
Editores de texto básicos.

```bash
nano archivo.txt
```


### `echo`
Escribe texto en la terminal o en un archivo.

```bash
echo "Hola"
echo "Hola" > saludo.txt
```

### `>` y `>>`
Redirección de salida.

```bash
ls > contenido.txt
echo "mensaje" >> log.txt
```

---

## 8. permisos y seguridad

En Linux cada archivo tiene permisos de lectura, escritura y ejecución.

```bash
ls -l
```

Ejemplo:

```bash
-rwxr-xr-x
```

Significa:

- `r` = lectura
- `w` = escritura
- `x` = ejecución

### Cambiar permisos

```bash
chmod +x programa
chmod 755 script.sh
```

### Propietario y grupo

```bash
ls -l
chown usuario:grupo archivo.txt
```

Esto es muy importante cuando trabajamos en servidores y máquinas remotas.

---

## 9. información del sistema

### `whoami`
Muestra el usuario actual.

```bash
whoami
```

### `who`
Muestra quién está conectado.

```bash
who
```

### `uname`
Muestra información del sistema.

```bash
uname -a
```

### `hostname`
Muestra el nombre de la máquina.

```bash
hostname
```

### `date`
Muestra la fecha y hora.

```bash
date
```

### `man`
Manual de un comando.

```bash
man ls
```

---

## 10. Acceso remoto a una máquina Linux

En esta asignatura vamos a trabajar con una máquina remota Linux, normalmente a través de una conexión SSH o mediante un cliente gráfico como MobaXterm.

### Máquina de prácticas

La primera práctica de la asignatura se conecta a la máquina remota llamada `aloe`.

- Nombre: `aloe.ac.uma.es`
- Dirección IP: `150.214.109.24`

En la práctica se utiliza **MobaXterm** como cliente de acceso remoto.

### Qué hace MobaXterm

MobaXterm permite:

- abrir una terminal remota en Linux,
- conectarse mediante SSH,
- ejecutar comandos en una máquina remota,
- mostrar ventanas gráficas si es necesario,
- facilitar el trabajo con sistemas distribuidos.

### Ejemplo de conexión

Desde una terminal local, lo más habitual es:

```bash
ssh usuario@aloe.ac.uma.es
```

Si la conexión es vía MobaXterm, se configura:

- Host: `aloe.ac.uma.es`
- Usuario: `ac001`, `ac002`, ...
- Puerto: normalmente 22

### Ejemplo de sesión remota

```bash
whoami
pwd
ls
uname -a
```

Esto nos permite trabajar como si estuvieramos en el equipo Linux remoto, aunque físicamente estemos en nuestro portátil o PC.

---

## 11. Primeros comandos que se suelen usar en práctica

Estos son los comandos más útiles para empezar:

```bash
whoami
pwd
ls
ls -l
ls -a
cd /
cd ~
mkdir pruebas
cd pruebas
touch archivo.txt
echo "Hola mundo" > archivo.txt
cat archivo.txt
cp archivo.txt copia.txt
mv copia.txt nuevo.txt
rm nuevo.txt
```

### Ejemplo de una secuencia sencilla

```bash
pwd
whoami
ls
mkdir mispruebas
cd mispruebas
touch saludo.txt
echo "Buenos días" > saludo.txt
cat saludo.txt
```

---

## 12. Comparación rápida con PowerShell en Windows

Aunque la base es la misma, el usuario suele estar acostumbrado a Windows.

### Windows (PowerShell)

```powershell
Get-Location
Get-ChildItem
New-Item -ItemType Directory -Name pruebas
Set-Location .\pruebas
New-Item -ItemType File -Name archivo.txt
Get-Content archivo.txt
Copy-Item archivo.txt copia.txt
Move-Item copia.txt nuevo.txt
Remove-Item nuevo.txt
```

### Linux (bash)

```bash
pwd
ls
mkdir pruebas
cd pruebas
touch archivo.txt
cat archivo.txt
cp archivo.txt copia.txt
mv copia.txt nuevo.txt
rm nuevo.txt
```

La lógica es equivalente: listar, entrar, crear, mover, borrar y leer archivos.

---

## 13. Qué veremos en la primera práctica

La primera práctica de la asignatura ya incorpora muchos de estos conceptos:

- conectarse a la máquina remota,
- entender usuarios y entornos Unix,
- navegar por directorios,
- crear y mover archivos,
- compilar programas simples en C,
- empezar a diferenciar entre compilación nativa y compilación cruzada,
- comprender qué es un ejecutable y cómo se genera.

Por tanto, esta sesión sirve como base para lo que se verá en la práctica 1.

---

## 14. Ejercicios recomendados para la clase

### Ejercicio 1: navegar por el sistema

```bash
pwd
ls
ls -a
cd /
ls
cd /home
ls
```

### Ejercicio 2: crear una estructura

```bash
mkdir -p ~/practica1/ejercicios
cd ~/practica1
ls
```

### Ejercicio 3: crear y leer archivos

```bash
cd ~/practica1
touch hola.txt
echo "Hola, mundo Linux" > hola.txt
cat hola.txt
```

### Ejercicio 4: copiar y mover

```bash
cp hola.txt copia.txt
mv copia.txt ejemplo.txt
ls
```

### Ejercicio 5: conectar a la máquina remota

```bash
ssh usuario@aloe.ac.uma.es
```

---

## 15. Consejos importantes para empezar

- No tengas miedo a equivocarte: la terminal suele ser muy permisiva, pero siempre se puede corregir.
- Lee siempre el mensaje de error: normalmente te dice exactamente qué está fallando.
- Usa `man` para consultar comandos.
- Empieza con cosas simples: `pwd`, `ls`, `cd`, `mkdir`, `cp`, `mv`.
- La práctica hace al experto.

---

## 16. Resumen

La línea de comandos es una herramienta fundamental para cualquier estudiante de arquitectura de computadores, sistemas, programación y computación aplicada.

En esta sesión hemos visto:

- qué es la estructura del sistema de archivos,
- cómo funcionan los paths,
- qué tipos de archivos existen,
- cómo se trabaja en Linux y cómo se compara con Windows,
- cómo conectarse a una máquina remota,
- cuáles son los primeros comandos útiles para empezar.

Con esta base, ya estamos preparados para entrar en la primera práctica de la asignatura: conexión remota, primeros pasos con Linux y compilación básica.

---

## 17. Enlaces útiles

- [Campus Virtual](https://eii.cv.uma.es/course/view.php?id=193)
- [Repositorio de la asignatura](https://github.com/luisfromero/arqcom)
- [MobaXterm](https://mobaxterm.mobatek.net/download-home-edition.html)
- [GNU GCC](https://gcc.gnu.org/)

---

## 18. Preguntas rápidas

### ¿Por qué es importante aprender línea de comandos?
Porque es la forma de acceso más directa a Linux, a sistemas remotos, a servidores, a compiladores y a herramientas esenciales para la ingeniería.

### ¿Linux y Windows son muy distintos?
La lógica es similar, pero Linux usa una jerarquía de carpetas y permisos mucho más explícita.

### ¿Es obligatorio usar la terminal?
No en todos los casos, pero sí es imprescindible para trabajar con sistemas remotos, compilación y administración básica.

### ¿Se puede aprender poco a poco?
Sí. Lo más importante es practicar un poco cada día y no tener miedo a cometer errores.
