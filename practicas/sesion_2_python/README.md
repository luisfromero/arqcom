# Introduccion a Python (Sesion 2)

Este material esta pensado para una sesion de **1 hora y 20 minutos** con alumnado que viene de MATLAB, con poca experiencia en programacion general y una base pequena de C.

Objetivo general de la sesion:

1. Dejar Python instalado y funcionando en Windows.
2. Trabajar desde VS Code de forma comoda.
3. Entender que son `pip`, modulos y paquetes.
4. Ejecutar y editar notebooks con Jupyter.
5. Conectar ideas de estructuras en C/C++ con clases y objetos (concepto general, no atado a un lenguaje).

---

## 0) Plan temporal sugerido (80 min)

1. **0-10 min**: contexto (Python vs MATLAB, ficheros `.py` y notebooks).
2. **10-30 min**: instalacion en Windows + estructura de carpetas en `C:\dev`.
3. **30-45 min**: VS Code + extensiones + seleccion de interprete.
4. **45-60 min**: `pip`, modulos y primer script reutilizable.
5. **60-72 min**: Jupyter en VS Code.
6. **72-80 min**: puente conceptual C struct -> C++ struct -> clase/objeto.

---

## 1) Contexto rapido para alumnos que vienen de MATLAB

En MATLAB, muchas veces trabajamos en scripts con arrays/matrices y funciones de alto nivel.

En Python vamos a usar dos formatos:

1. **Script `.py`**: fichero de codigo ejecutable.
2. **Notebook `.ipynb`**: documento por celdas (texto + codigo + salidas), ideal para aprender y experimentar.

Idea clave:

- MATLAB suele llevaros a un entorno muy integrado.
- Python os da mas libertad, pero hay que entender mejor el entorno (interprete, paquetes, rutas, etc.).

---

## 2) Instalacion en Windows (trabajando en `C:\dev`)

### 2.1 Estructura de carpetas recomendada

Crear una carpeta base para desarrollo:

```powershell
mkdir C:\dev
mkdir C:\dev\acpracticas
```

Trabajaremos desde ahi para evitar rutas largas y problemas de permisos en carpetas del sistema.

### 2.2 Instalar Python

1. Ir a la web oficial: [https://www.python.org/downloads/windows/](https://www.python.org/downloads/windows/)
2. Descargar Python 3.x (version estable reciente).
3. En el instalador, marcar:
   - `Add python.exe to PATH`
   - opcion de instalacion para el usuario actual (normalmente suficiente)
4. Completar instalacion.

### 2.3 Comprobaciones en terminal

Abrir PowerShell y ejecutar:

```powershell
python --version
pip --version
```

Si `python` falla, probar:

```powershell
py --version
py -m pip --version
```

Regla practica para Windows:

- Si hay duda, usar `py -m pip ...` en lugar de `pip ...`.

---

## 3) Acceso desde VS Code a Python

### 3.1 Extensiones necesarias

Instalar en VS Code:

1. **Python** (Microsoft)
2. **Jupyter** (Microsoft)

### 3.2 Abrir proyecto

1. `File -> Open Folder`
2. Abrir `C:\dev\acpracticas` (o vuestra carpeta de trabajo equivalente)

### 3.3 Seleccionar interprete Python

1. `Ctrl+Shift+P`
2. Buscar: `Python: Select Interpreter`
3. Elegir el Python 3 instalado.

Si no aparece, cerrar y abrir VS Code tras la instalacion de Python.

### 3.4 Primera prueba en VS Code

Crear `hola.py`:

```python
print("Hola desde Python en VS Code")
```

Ejecutar con el boton de Run o desde terminal:

```powershell
python hola.py
```

---

## 4) `pip`, modulos y paquetes

### 4.1 Que es `pip`

`pip` es el gestor de paquetes de Python. Sirve para instalar librerias externas (por ejemplo `numpy`, `matplotlib`, `pandas`).

### 4.2 Instalaciones basicas

```powershell
py -m pip install --upgrade pip
py -m pip install numpy matplotlib jupyter
```

### 4.3 Ver paquetes instalados

```powershell
py -m pip list
```

### 4.4 Modulo propio (idea de reutilizacion)

Crear `operaciones.py`:

```python
def suma(a, b):
	return a + b

def media(valores):
	return sum(valores) / len(valores)
```

Crear `main.py`:

```python
from operaciones import suma, media

print(suma(2, 3))
print(media([3, 5, 7]))
```

Concepto:

- Un **modulo** es un fichero `.py` importable.
- Un **paquete** es una coleccion de modulos (normalmente en una carpeta).

---

## 5) Jupyter en VS Code

### 5.1 Por que interesa para esta asignatura

Ventajas para principiantes:

1. Ejecutar por bloques (celdas).
2. Ver resultados inmediatamente.
3. Mezclar explicacion y codigo.

### 5.2 Uso minimo

1. Abrir un `.ipynb` del repositorio.
2. Seleccionar kernel Python correcto.
3. Ejecutar celda a celda.

Si faltan dependencias, instalarlas con:

```powershell
py -m pip install <paquete>
```

### 5.3 Notebooks de esta sesion

Puedes revisar estos materiales:

1. [intro_python.ipynb](intro_python.ipynb)
2. [introduccion_python.ipynb](introduccion_python.ipynb)
3. [Leccion 1 (Apuntes) - Programacion estructurada vs POO.ipynb](Lección%201%20(Apuntes)%20-%20Programación%20estructurada%20vs%20POO.ipynb)

---

## 6) Puente conceptual: de C a C++ y de ahi a clases/objetos

Esta parte es clave para entender por que existe la POO y que problema resuelve.

### 6.1 Estructura en C: datos agrupados

En C, una estructura agrupa datos relacionados:

```c
typedef struct {
	double x;
	double y;
} Point;
```

Esto permite representar un punto 2D como una sola unidad de datos.

Para calcular distancia, en C tipicamente usamos una funcion externa:

```c
double distance(Point a, Point b) {
	double dx = a.x - b.x;
	double dy = a.y - b.y;
	return sqrt(dx*dx + dy*dy);
}
```

Idea:

- Los datos (`x`, `y`) viven en la estructura.
- El comportamiento (`distance`) vive fuera.

### 6.2 Estructura en C++: datos + comportamiento

En C++, una `struct` puede incluir metodos:

```cpp
struct Point {
	double x;
	double y;

	double distance(const Point& other) const {
		double dx = x - other.x;
		double dy = y - other.y;
		return std::sqrt(dx*dx + dy*dy);
	}
};
```

Ahora el comportamiento esta "pegado" al dato que le corresponde.

### 6.3 Salto a clase y objeto (concepto general)

Sin casarnos con un lenguaje:

1. **Clase**: plantilla o definicion de un tipo con estado y comportamiento.
2. **Objeto**: instancia concreta de esa clase en memoria.

Aplicado al ejemplo:

1. Clase `Point` define que un punto tiene coordenadas y sabe calcular distancias.
2. Objetos `p1` y `p2` son puntos concretos (por ejemplo `(1,2)` y `(4,6)`).

Beneficios para alumnado:

1. Organizacion del codigo en entidades del dominio.
2. Menos errores por mezclar datos no relacionados.
3. Mejor reutilizacion y mantenimiento en proyectos medianos/grandes.

Mensaje pedagogico importante:

- POO no es "mas dificil porque si".
- Surge para gestionar complejidad cuando el codigo crece.

---

## 7) Mini guion de pizarra (10 min finales)

1. Dibujar dos puntos en 2D.
2. Preguntar: "que datos necesito para un punto?" -> `x`, `y`.
3. Preguntar: "que operaciones necesito?" -> distancia, mover, etc.
4. Mostrar enfoque C (datos en `struct`, funciones sueltas).
5. Mostrar enfoque orientado a objetos (datos + operaciones en la misma entidad).
6. Cerrar con: "una clase modela conceptos del problema real".

---

## 8) Problemas comunes y solucion rapida

1. `python` no se reconoce:
   - Cerrar/abrir terminal.
   - Probar `py --version`.
   - Revisar instalacion con opcion PATH.
2. VS Code no detecta Python:
   - Reinstalar extension Python.
   - `Python: Select Interpreter`.
3. `pip` instala pero no importa:
   - El paquete se instalo en otro interprete.
   - Usar siempre `py -m pip install ...` y verificar interprete en VS Code.
4. Notebook no ejecuta:
   - Verificar kernel.
   - Instalar `jupyter` en ese mismo interprete.

---

## 9) Material relacionado

1. [Breve Introduccion a GIT](GIT.md)
2. Notebooks de la carpeta para practicar paso a paso.

---

## 10) Cierre de la sesion

Al final de esta clase, cada alumno deberia poder:

1. Ejecutar scripts Python en su Windows.
2. Abrir y correr notebooks en VS Code.
3. Instalar un paquete con `pip` y usarlo.
4. Explicar la diferencia entre una estructura de datos y una clase/objeto a nivel conceptual.