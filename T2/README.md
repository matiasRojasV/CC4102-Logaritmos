# Tarea 2: árboles B y Bε

Implementación en C++ de un árbol B y dos configuraciones de árboles Bε.
El programa permite comparar el costo de construir los árboles y realizar
búsquedas por rango sobre datos de temperatura almacenados en memoria externa.

## Requisitos

- Linux, macOS o un sistema compatible.
- `g++` con soporte para C++17.
- `make`.
- Un archivo de datos binario llamado `datos.bin`.

## Compilación

Desde esta carpeta, ejecutar:

```bash
make
```

Esto genera el ejecutable `T2`.

## Ejecución

Ejecutar el programa indicando el archivo de datos y, opcionalmente, el
directorio donde se guardarán los resultados:

```bash
./T2 datos.bin
./T2 datos.bin out
```

El directorio de salida debe existir antes de ejecutar el programa. Si no se
indica, se utiliza la carpeta actual.

El programa ejecuta automáticamente:

- experimentos con un árbol B;
- experimentos con árboles Bε usando las configuraciones aproximadas
  `ε = 0.25` y `ε = 0.75`;
- un experimento adicional variando el parámetro `α`.

## Archivos generados

En el directorio de salida se generan:

- `experiment_results.csv`: resultados de las comparaciones principales.
- `alpha_bonus_results.csv`: resultados del experimento con distintos valores
  de `α`.
- `btree.bin`, `beps025.bin` y `beps075.bin`: árboles serializados en formato
  binario.

Cada nodo ocupa un bloque de 4096 bytes. Las búsquedas por rango son inclusivas,
es decir, consideran las llaves dentro de `[l, u]`.

## Estructura del proyecto

```text
T2/
├── include/   # Definiciones de estructuras y clases
├── src/       # Implementación y programa principal
├── out/       # Resultados y árboles generados
├── Makefile   # Comandos de compilación y limpieza
└── datos.bin  # Archivo de entrada binario
```

Para eliminar el ejecutable y los archivos binarios generados, ejecutar:

```bash
make clean
```

## Generación de gráficos

El script `python/plot.py` utiliza los archivos `experiment_results.csv` y
`alpha_bonus_results.csv` para generar las figuras del informe. Se requieren
Python 3 y las bibliotecas `pandas` y `matplotlib`.

Desde la carpeta del proyecto, ejecutar indicando primero la carpeta de los CSV
y luego la carpeta de salida:

```bash
python3 python/plot.py out/test2 python/plot
```

Las figuras SVG se guardan en la carpeta de salida:

- `insertion_cost.svg`: escrituras y tiempo de construcción.
- `search_cost.svg`: lecturas de disco y tiempo de búsqueda.
- `alpha_analysis.svg`: efecto de `alpha` sobre las escrituras y lecturas.
