#include "PrimAlgorithm.h"
#include <chrono>
#include <limits>
#include <cmath>

MST PrimAlgorithm::primBinomial(const Graph& graph) {
    int v = graph.getNumVertices();
    
    MST result;
    result.numOperations = 0;
    result.totalWeight = 0.0;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Inicializar distancias
    std::vector<double> key(v, std::numeric_limits<double>::infinity());
    std::vector<bool> inMST(v, false);
    std::vector<int> parent(v, -1);
    
    BinomialHeap pq;
    
    // Comenzar desde el vértice 0
    key[0] = 0.0;
    for (int i = 0; i < v; i++) {
        pq.insert(i, key[i]);
        result.numOperations++;
    }
    
    while (!pq.isEmpty()) {
        int u = pq.extractMin();
        result.numOperations++;
        
        if (u == -1 || inMST[u]) break;
        
        inMST[u] = true;
        
        if (parent[u] != -1) {
            result.edges.push_back({parent[u], u});
            result.totalWeight += key[u];
        }
        
        for (const auto& [v_neighbor, weight] : graph.getAdjacency(u)) {
            if (!inMST[v_neighbor] && weight < key[v_neighbor]) {
                key[v_neighbor] = weight;
                parent[v_neighbor] = u;
                pq.decreaseKey(v_neighbor, weight);
                result.numOperations++;
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

MST PrimAlgorithm::primFibonacci(const Graph& graph) {
    int v = graph.getNumVertices();
    
    MST result;
    result.numOperations = 0;
    result.totalWeight = 0.0;

    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Inicializar distancias
    std::vector<double> key(v, std::numeric_limits<double>::infinity());
    std::vector<bool> inMST(v, false);
    std::vector<int> parent(v, -1);
    
    FibonacciHeap pq;
    
    // Comenzar desde el vértice 0
    key[0] = 0.0;
    for (int i = 0; i < v; i++) {
        pq.insert(i, key[i]);
        result.numOperations++;
    }
    
    while (!pq.isEmpty()) {
        int u = pq.extractMin();
        result.numOperations++;
        
        if (u == -1 || inMST[u]) break;
        
        inMST[u] = true;
        
        if (parent[u] != -1) {
            result.edges.push_back({parent[u], u});
            result.totalWeight += key[u];
        }
        
        for (const auto& [v_neighbor, weight] : graph.getAdjacency(u)) {
            if (!inMST[v_neighbor] && weight < key[v_neighbor]) {
                key[v_neighbor] = weight;
                parent[v_neighbor] = u;
                pq.decreaseKey(v_neighbor, weight);
                result.numOperations++;
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

bool PrimAlgorithm::verifyMST(const MST& mst1, const MST& mst2) {
    const double epsilon = 1e-9;
    return std::abs(mst1.totalWeight - mst2.totalWeight) < epsilon;
}
