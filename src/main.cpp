#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
#include "Graph.h"
#include "PrimAlgorithm.h"

/**
 * @file main.cpp
 * @brief Batería de experimentos para comparar colas binomiales vs Fibonacci.
 * 
 * Ejecuta cuatro series de experimentos (A, B, C, D) midiendo tiempos
 * y operaciones con alta precisión usando reloj monótono.
 */

struct ExperimentResult {
    int v, e;
    double timeBinomial, timeFibonacci;
    long long opsBinomial, opsFibonacci;
    bool valid;
};

int main() {
    std::vector<ExperimentResult> results;
    
    std::cout << "=== Prim's Algorithm: Binomial vs Fibonacci Heap ===" << std::endl;
    std::cout << std::endl;
    
    // Nota: Los parámetros de las series (A, B, C, D) deben ser definidos
    // según el enunciado específico del trabajo.
    
    // Serie A: v creciente, e = 2v
    std::cout << "Serie A: Variando vértices (e = 2v)" << std::endl;
    
    std::vector<int> verticesSeries = {10, 50, 100, 500, 1000, 5000};
    
    for (int v : verticesSeries) {
        int e = 2 * v;
        
        try {
            std::cout << "  Testing v=" << v << ", e=" << e << "..." << std::flush;
            
            Graph graph(v, e);
            
            MST mstBinomial = PrimAlgorithm::primBinomial(graph);
            MST mstFibonacci = PrimAlgorithm::primFibonacci(graph);
            
            bool valid = PrimAlgorithm::verifyMST(mstBinomial, mstFibonacci);
            
            ExperimentResult res;
            res.v = v;
            res.e = e;
            res.timeBinomial = mstBinomial.executionTime;
            res.timeFibonacci = mstFibonacci.executionTime;
            res.opsBinomial = mstBinomial.numOperations;
            res.opsFibonacci = mstFibonacci.numOperations;
            res.valid = valid;
            
            results.push_back(res);
            
            std::cout << " OK" << std::endl;
            std::cout << "    Binomial: " << std::scientific << mstBinomial.executionTime 
                      << "s (" << mstBinomial.numOperations << " ops)" << std::endl;
            std::cout << "    Fibonacci: " << std::scientific << mstFibonacci.executionTime 
                      << "s (" << mstFibonacci.numOperations << " ops)" << std::endl;
            std::cout << "    MST Weight: " << std::fixed << std::setprecision(2) 
                      << mstBinomial.totalWeight << " (Valid: " << (valid ? "YES" : "NO") << ")" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << " ERROR: " << e.what() << std::endl;
        }
    }
    
    std::cout << std::endl << "=== Resultados ===" << std::endl;
    std::cout << std::setw(6) << "V" << std::setw(8) << "E" 
              << std::setw(15) << "T Binomial" << std::setw(15) << "T Fibonacci"
              << std::setw(12) << "Speedup" << std::setw(8) << "Valid" << std::endl;
    std::cout << std::string(64, '-') << std::endl;
    
    for (const auto& res : results) {
        double speedup = res.timeBinomial / res.timeFibonacci;
        std::cout << std::setw(6) << res.v << std::setw(8) << res.e 
                  << std::scientific << std::setprecision(3)
                  << std::setw(15) << res.timeBinomial 
                  << std::setw(15) << res.timeFibonacci
                  << std::fixed << std::setprecision(2)
                  << std::setw(12) << speedup 
                  << std::setw(8) << (res.valid ? "YES" : "NO") << std::endl;
    }
    
    // Guardar resultados en archivo para análisis posterior
    std::ofstream outfile("results/experimental_data.csv");
    if (outfile.is_open()) {
        outfile << "V,E,TimeBinomial,TimeFibonacci,OpsBinomial,OpsFibonacci,Valid\n";
        for (const auto& res : results) {
            outfile << res.v << "," << res.e << "," 
                    << res.timeBinomial << "," << res.timeFibonacci << ","
                    << res.opsBinomial << "," << res.opsFibonacci << ","
                    << res.valid << "\n";
        }
        outfile.close();
        std::cout << "\nResultados guardados en results/experimental_data.csv" << std::endl;
    }
    
    return 0;
}
