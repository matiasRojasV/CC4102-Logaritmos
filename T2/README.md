# Tarea 2: árboles B y Bε

## Compilación

Se requiere un compilador compatible con C++17:

```sh
make
```

## Ejecución

`datos.bin` debe contener pares consecutivos `(int32_t, float)` en formato binario.

```sh
./tarea2 datos.bin N [directorio_salida]
```

El programa inserta los primeros `N` pares en:

- un árbol B con `b = 340`;
- un árbol Bε con `c = 4`, `lmax = 506` (ε aproximadamente 0.25);
- un árbol Bε con `c = 126`, `lmax = 384` (ε aproximadamente 0.75).

Cada nodo ocupa exactamente 4096 bytes en los archivos generados. El programa
reporta la cantidad de nodos y escrituras simuladas durante la construcción.

La interfaz de las clases también permite ejecutar búsquedas inclusivas
`[l, u]` mediante `rangeSearch`. Las búsquedas leen los nodos directamente
desde el archivo y actualizan el contador de lecturas de bloques.