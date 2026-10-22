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
