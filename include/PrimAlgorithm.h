#ifndef PRIM_ALGORITHM_H
#define PRIM_ALGORITHM_H

#include "Graph.h"
#include "BinomialHeap.h"
#include "FibonacciHeap.h"
#include <vector>
#include <utility>

/**
 * @struct MST
 * @brief Resultado del algoritmo de Prim (Minimum Spanning Tree).
 */
struct MST {
    std::vector<std::pair<int, int>> edges;  // Aristas del MST
    double totalWeight;                       // Peso total
    long long numOperations;                  // Número de operaciones realizadas
    double executionTime;                     // Tiempo de ejecución
};

/**
 * @class PrimAlgorithm
 * @brief Implementación del algoritmo de Prim con diferentes colas de prioridad.
 */
class PrimAlgorithm {
public:
    /**
     * Ejecuta el algoritmo de Prim usando una cola binomial.
     * 
     * @param graph Grafo de entrada
     * @return Estructura MST con el árbol generador mínimo
     * 
     * Complejidad: O(e log v)
     */
    static MST primBinomial(const Graph& graph);
    
    /**
     * Ejecuta el algoritmo de Prim usando una cola de Fibonacci.
     * 
     * @param graph Grafo de entrada
     * @return Estructura MST con el árbol generador mínimo
     * 
     * Complejidad: O(e + v log v)
     */
    static MST primFibonacci(const Graph& graph);
    
    /**
     * Verifica que dos MST tengan el mismo peso.
     * 
     * @param mst1 Primer MST
     * @param mst2 Segundo MST
     * @return true si tienen el mismo peso (dentro de tolerancia numérica)
     */
    static bool verifyMST(const MST& mst1, const MST& mst2);
};

#endif // PRIM_ALGORITHM_H
