#include "Graph.h"
#include <iostream>
#include <algorithm>

Graph::Graph(int v, int e) : numVertices(v), numEdges(0) {
    if (e < v - 1) {
        throw std::invalid_argument("Number of edges must be at least v-1 for connectivity");
    }
    
    adj.resize(v);
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> weightDist(1.0, 100.0);
    
    // Árbol cobertor (O(V) y cero memoria extra)
    for (int i = 1; i < v; i++) {
        std::uniform_int_distribution<> dist(0, i - 1);
        int target = dist(gen);
        double weight = weightDist(gen);
        
        adj[i].push_back({target, weight});
        adj[target].push_back({i, weight});
        numEdges++;
    }
    
    // Añadir aristas restantes aleatorias
    std::uniform_int_distribution<> vertexDist(0, v - 1);
    
    while (numEdges < e) {
        int u = vertexDist(gen);
        int v_node = vertexDist(gen);
        
        if (u == v_node) continue; // Evitar reflexivas
        
        // Búsqueda lineal en el vector (rapidísimo por caché, sin requerir std::set)
        bool exists = false;
        for (const auto& edge : adj[u]) {
            if (edge.first == v_node) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            double weight = weightDist(gen);
            adj[u].push_back({v_node, weight});
            adj[v_node].push_back({u, weight});
            numEdges++;
        }
    }
}

Graph::~Graph() = default;

int Graph::getNumVertices() const { return numVertices; }
int Graph::getNumEdges() const { return numEdges; }

const std::vector<std::pair<int, double>>& Graph::getAdjacency(int u) const {
    return adj[u];
}

void Graph::printGraph() const {
    for (int u = 0; u < numVertices; u++) {
        std::cout << "Vertex " << u << ": ";
        for (const auto& [v, weight] : adj[u]) {
            std::cout << "(" << v << ", " << weight << ") ";
        }
        std::cout << "\n";
    }
}