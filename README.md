# Prim's Algorithm: Binomial vs Fibonacci Heap

## Descripción

Este proyecto implementa el algoritmo de Prim para encontrar el árbol generador mínimo (MST) de un grafo usando dos estructuras de datos diferentes:

- **Cola Binomial** (complejidad: O(e log v))
- **Cola de Fibonacci** (complejidad: O(e + v log v))

El objetivo es comparar empíricamente el desempeño de ambas estructuras y validar las diferencias teóricas de complejidad.

## Estructura del Proyecto

```
.
├── include/             # Archivos header
│   ├── Graph.h          # Generador y estructura del grafo
│   ├── BinomialHeap.h   # Cola binomial
│   ├── FibonacciHeap.h  # Cola de Fibonacci
│   └── PrimAlgorithm.h  # Algoritmo de Prim
├── src/                 # Archivos de implementación
│   ├── Graph.cpp
│   ├── BinomialHeap.cpp
│   ├── FibonacciHeap.cpp
│   ├── PrimAlgorithm.cpp
│   └── main.cpp        # Batería de experimentos
├── build/              # Directorio de compilación
├── results/            # Resultados de los experimentos (CSV)
├── report/             # Informe en PDF/LaTeX
├── data/               # Datos de entrada (si es necesario)
├── README.md           # Este archivo
└── Makefile            # Compilación

```

## Requisitos

- **Compilador:** g++ 11.0 o superior (con soporte C++20)
- **Sistema Operativo:** Linux/macOS/Windows (MinGW)
- **Librerías:** Ninguna (sin dependencias externas)

## Compilación

### Opción 1: Usando Makefile

```bash
make clean
make
```

### Opción 2: Compilación manual

```bash
cd build
g++ -std=c++20 -O3 -D_GLIBCXX_DEBUG \
    ../src/Graph.cpp \
    ../src/BinomialHeap.cpp \
    ../src/FibonacciHeap.cpp \
    ../src/PrimAlgorithm.cpp \
    ../src/main.cpp \
    -I../include \
    -o prim_experiment
```

### Opción 3: Usando CMake

```bash
mkdir build && cd build
cmake ..
make
```

## Ejecución

```bash
./build/prim_experiment
```

El programa ejecutará automáticamente la batería de experimentos y generará un archivo `results/experimental_data.csv` con los resultados.

## Componentes Principales

### Graph (Graph.h / Graph.cpp)
- Generación de grafos aleatorios conexos
- Estructura de listas de adyacencia
- Garantiza evitar aristas reflexivas y duplicadas

### BinomialHeap (BinomialHeap.h / BinomialHeap.cpp)
- Operación `extractMin`: O(log n)
- Operación `decreaseKey`: O(log n)
- Operación `insert`: O(1) amortizado

### FibonacciHeap (FibonacciHeap.h / FibonacciHeap.cpp)
- Operación `extractMin`: O(log n) amortizado
- Operación `decreaseKey`: O(1) amortizado
- Operación `insert`: O(1)
- Implementa cascading cuts para mantener la propiedad heap

### PrimAlgorithm (PrimAlgorithm.h / PrimAlgorithm.cpp)
- `primBinomial`: Ejecuta Prim con cola binomial
- `primFibonacci`: Ejecuta Prim con cola de Fibonacci
- `verifyMST`: Verifica que ambos generen árboles con igual peso

## Experimentación

El programa suporta varias series de experimentos:

- **Serie A:** Vértices crecientes (e = 2v)
- **Serie B:** Aristas crecientes (v fijo)
- **Serie C:** Medición de operaciones amortizadas (Binomial)
- **Serie D:** Medición de operaciones amortizadas (Fibonacci)

Cada serie genera múltiples instancias de grafos y promedia los tiempos de ejecución.

## Validación

El programa verifica que ambas implementaciones generen MST con el mismo peso total, lo que garantiza la correctitud del algoritmo.

## Medición de Tiempos

Se utiliza `std::chrono::high_resolution_clock` para obtener tiempos de alta resolución sin interferir con la ejecución.

Las mediciones se almacenan en memoria y se escriben al archivo CSV solo después de la ejecución.

## Resultados

Los resultados se guardan en `results/experimental_data.csv` con el siguiente formato:

```
V,E,TimeBinomial,TimeFibonacci,OpsBinomial,OpsFibonacci,Valid
10,20,0.0001234,0.0001234,150,150,1
50,100,0.0005678,0.0004567,750,750,1
...
```

## Troubleshooting

### Error de compilación relacionado con `std::iota`
Asegúrate de incluir `<numeric>` en Graph.cpp:
```cpp
#include <numeric>
```

### Advertencias de debug
Se recomienda compilar con `-D_GLIBCXX_DEBUG` solo durante desarrollo. Para producción, omitir este flag.

## Autoría

[Tu nombre]
Curso: CC4102 - Diseño y Análisis de Algoritmos
Universidad: [Tu Universidad]

