# Tarea 1 - Algoritmo de Prim y Análisis Amortizado

Este proyecto implementa y compara empíricamente el desempeño del algoritmo de Prim para encontrar el Árbol Generador Mínimo (MST) de un grafo, utilizando dos estructuras de datos diferentes: la Cola Binomial y la Cola de Fibonacci. El objetivo principal es validar las diferencias teóricas de complejidad mediante la evaluación de tiempos de ejecución totales y costos amortizados bajo distintas densidades de grafos generados aleatoriamente.

## Estructura del Proyecto

```text
.
├── include/                # Archivos header (.h)
│   ├── Graph.h             
│   ├── BinomialHeap.h      
│   ├── FibonacciHeap.h     
│   └── PrimAlgorithm.h     
├── src/                    # Archivos de implementación (.cpp)
│   ├── Graph.cpp
│   ├── BinomialHeap.cpp
│   ├── FibonacciHeap.cpp
│   ├── PrimAlgorithm.cpp
│   └── main.cpp            # Batería de experimentos
├── python/
│   └── genPlots.py         # Script de visualización
├── build/                  # Directorio de compilación
├── README.md               # Este archivo
└── Makefile                # Configuración para Make
```

## Requisitos

- **Compilador:** g++ compatible con el estándar C++20 (-std=c++20).   
- **Herramientas de construcción:** make   
- **Python:** Versión 3.x con las dependencias listadas en requirements.txt (numpy, pandas, matplotlib).


## Compilación y Ejecución (C++)

### Usando Makefile

```bash
make clean
make run
```

Si deseas compilar en modo de depuración (agregando -g y -D_GLIBCXX_DEBUG), puedes ejecutar:

```bash
make debug
```


## Ejecución

```bash
./build/prim_experiment
```

## Resultados

Al ejecutar el binario `prim_experiment`, el programa escribirá los promedios de tiempos de ejecución y recuento de operaciones en el archivo `results_experimental.csv` en el directorio raíz. Este archivo contiene columnas detalladas como TimeBinomial, `TimeFibonacci`, `OpsBinomial`, `OpsFibonacci` y el estado de validación `Valid`

Para generar los gráficos comparativos de las cotas teóricas vs. empíricas, ejecuta el script de Python:

```bash
# 1. Crear el entorno virtual
python3 -m venv .venv

# 2. Activar el entorno virtual
# En Linux/macOS:
source venv/bin/activate
# En Windows:
venv\Scripts\activate

# 3. Instalar las dependencias
pip install -r requirements.txt

# 4. Generar los gráficos
python genPlots.py
```

Esto procesará los datos de `results_experimental.csv` y creará una carpeta llamada plots con los gráficos

## Componentes Principales

### Graph (Graph.h / Graph.cpp)

Clase encargada de generar grafos conexos aleatorios utilizando listas de adyacencia. Garantiza una conectividad mínima de $V-1$ aristas y evita la creación de aristas reflexivas o duplicadas. 


### BinomialHeap (BinomialHeap.h / BinomialHeap.cpp)

Implementa una cola de prioridad con tiempo amortizado $O(1)$ para mantener el mínimo, mientras que extractMin y decreaseKey operan en $O(\log n)$. Registra el historial de intercambios para análisis de operaciones.


### FibonacciHeap (FibonacciHeap.h / FibonacciHeap.cpp)

Estructura optimizada mediante cascading cuts que permite realizar decreaseKey en tiempo amortizado $O(1)$, y extractMin en tiempo amortizado $O(\log n)$.   


### PrimAlgorithm (PrimAlgorithm.h / PrimAlgorithm.cpp)

Ejecuta el cálculo del MST inyectando métricas de tiempo de ejecución de alta resolución y número de operaciones estructurales. Incorpora la función verifyMST para asegurar que ambas implementaciones entreguen árboles con el peso total equivalente.
