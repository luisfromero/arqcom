# main.py
import ctypes
from fastapi import FastAPI

# --- Carga de la biblioteca C y definición de la función ---

# 1. Cargar la biblioteca compartida que acabamos de compilar.
#    Se asume que está en el mismo directorio.
try:
    c_lib = ctypes.CDLL('./libfact.so')
except OSError as e:
    print("Error: No se pudo cargar la biblioteca 'libfact.so'.")
    print("Asegúrate de haberla compilado con: gcc -shared -fPIC -o libfact.so fact.c")
    exit()

# 2. Definir la "firma" de la función C: int fact(int x)
#    Argumentos que recibe: una lista de tipos ctypes.
c_lib.fact.argtypes = [ctypes.c_int]
# Valor que devuelve: un tipo ctypes.
c_lib.fact.restype = ctypes.c_int

# --- Creación de la API con FastAPI ---

app = FastAPI(
    title="API para función en C",
    description="Un ejemplo de cómo llamar a código C compilado desde una API de Python.",
    version="1.0.0"
)

@app.get("/fact/{number}", summary="Calcula el resultado usando la biblioteca C")
def get_fact_from_c(number: int):
    """
    Recibe un número entero, lo pasa a la función `fact` de la biblioteca C
    y devuelve el resultado.
    """
    # 3. Llamar a la función C como si fuera una función de Python.
    result = c_lib.fact(number)
    
    return {"input_number": number, "result_from_c": result}

@app.get("/")
def root():
    return {"message": "Bienvenido a la API. Prueba el endpoint /fact/{numero}, por ejemplo: /fact/10"}


@app.get("/nombre")
def root():
    return {"message": "Felipe"}
