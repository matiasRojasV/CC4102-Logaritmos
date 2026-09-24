#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <utility>
#include <random>
#include <unordered_set>

// Función hash personalizada para pares de enteros
struct EdgeHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        auto h1 = std::hash<int>{}(p.first);
        auto h2 = std::hash<int>{}(p.second);
        // Combinación de hashes usando bitwise XOR y shifts
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

/**
 * @class Graph
 * @brief Generador y estructura de grafo con listas de adyacencia.
 */
class Graph {
private:
    int numVertices;
    int numEdges;
    // Lista de adyacencia: adj[u] = {(v, weight), ...}
    std::vector<std::vector<std::pair<int, double>>> adj;
    
public:
    Graph(int v, int e);
    ~Graph();
    int getNumVertices() const;
    int getNumEdges() const;
    const std::vector<std::pair<int, double>>& getAdjacency(int u) const;
    void printGraph() const;
};

#endif // GRAPH_H