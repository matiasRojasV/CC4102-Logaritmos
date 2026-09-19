cmake_minimum_required(VERSION 3.10)
project(PrimAlgorithmComparison CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opciones de compilación
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -D_GLIBCXX_DEBUG")

# Directorios
include_directories(include)

# Archivos fuente
set(SOURCES
    src/Graph.cpp
    src/BinomialHeap.cpp
    src/FibonacciHeap.cpp
    src/PrimAlgorithm.cpp
    src/main.cpp
)

# Crear ejecutable
add_executable(prim_experiment ${SOURCES})

# Crear directorio de resultados
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/results")
