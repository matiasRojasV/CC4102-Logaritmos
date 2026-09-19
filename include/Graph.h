#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <utility>
#include <random>

/**
 * @class Graph
 * @brief Generador y estructura de grafo con listas de adyacencia.
 * 
 * Implementa un grafo no dirigido usando listas de adyacencia.
 * Proporciona métodos para generar grafos aleatorios conexos evitando
 * aristas reflexivas y repetidas.
 */
class Graph {
private:
    int numVertices;
    int numEdges;
    // Lista de adyacencia: adj[u] = {(v, weight), ...}
    std::vector<std::vector<std::pair<int, double>>> adj;
    
    /**
     * Crea un árbol cobertor aleatorio para garantizar conexidad.
     */
    void createSpanningTree(std::mt19937& gen);
    
public:
    /**
     * Constructor que genera un grafo aleatorio conexo.
     * 
     * @param v Número de vértices
     * @param e Número de aristas (debe ser >= v-1 para conexidad)
     */
    Graph(int v, int e);
    
    /**
     * Destructor.
     */
    ~Graph();
    
    /**
     * Retorna el número de vértices.
     */
    int getNumVertices() const;
    
    /**
     * Retorna el número de aristas.
     */
    int getNumEdges() const;
    
    /**
     * Retorna la lista de adyacencia del vértice u.
     * 
     * @param u Vértice origen
     * @return Vector de pares (destino, peso)
     */
    const std::vector<std::pair<int, double>>& getAdjacency(int u) const;
    
    /**
     * Imprime la estructura del grafo (para debug).
     */
    void printGraph() const;
};

#endif // GRAPH_H
