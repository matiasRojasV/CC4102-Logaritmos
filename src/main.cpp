#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include "Graph.h"
#include "PrimAlgorithm.h"

#ifdef _GLIBCXX_DEBUG
static constexpr int REPETITIONS = 3;
#else
static constexpr int REPETITIONS = 10;
#endif

// Función auxiliar para ejecutar varias repeticiones de una configuración y promediar
void ejecutarConfiguracion(int i, int j, const std::string& serie, std::ofstream& outfile) {
    int v = 1 << i; // v = 2^i
    int e = 1 << j; // e = 2^j
    
    std::cout << "Ejecutando " << serie << " (v=2^" << i << ", e=2^" << j << ") - "
              << REPETITIONS << " repeticiones..." << std::flush;
    
    double totalTimeBinomial = 0.0;
    double totalTimeFibonacci = 0.0;
    long long totalOpsBinomial = 0;
    long long totalOpsFibonacci = 0;
    bool allValid = true;
    
    for (int rep = 0; rep < REPETITIONS; rep++) {
        // Generar un grafo nuevo y distinto en cada repetición
        std::cout << "\n  [Rep " << rep << "] Generando grafo... " << std::flush;
        Graph graph(v, e);
        
        std::cout << "Listo. Binomial... " << std::flush;
        MST mstBinomial = PrimAlgorithm::primBinomial(graph);

        std::cout << "Listo. Fibonacci... " << std::flush;
        MST mstFibonacci = PrimAlgorithm::primFibonacci(graph);
        
        // Verificar que el peso del MST sea igual en ambas implementaciones
        if (!PrimAlgorithm::verifyMST(mstBinomial, mstFibonacci)) {
            allValid = false;
        }
        
        totalTimeBinomial += mstBinomial.executionTime;
        totalTimeFibonacci += mstFibonacci.executionTime;
        totalOpsBinomial += mstBinomial.numOperations;
        totalOpsFibonacci += mstFibonacci.numOperations;
        
        std::cout << "OK." << std::flush;
    }
    
    // Calcular promedios
    double avgTimeBinomial = totalTimeBinomial / REPETITIONS;
    double avgTimeFibonacci = totalTimeFibonacci / REPETITIONS;
    long long avgOpsBinomial = totalOpsBinomial / REPETITIONS;
    long long avgOpsFibonacci = totalOpsFibonacci / REPETITIONS;
    
    std::cout << " OK" << std::endl;
    std::cout << "  Binomial  : " << std::scientific << avgTimeBinomial << "s (" << avgOpsBinomial << " ops)" << std::endl;
    std::cout << "  Fibonacci : " << std::scientific << avgTimeFibonacci << "s (" << avgOpsFibonacci << " ops)" << std::endl;
    std::cout << "  Validez   : " << (allValid ? "Aprobada" : "FALLÓ") << "\n" << std::endl;
    
    // Guardar en el CSV
    if (outfile.is_open()) {
        outfile << serie << "," << i << "," << j << "," << v << "," << e << ","
                << std::fixed << std::setprecision(6) << avgTimeBinomial << "," 
                << avgTimeFibonacci << ","
                << avgOpsBinomial << "," << avgOpsFibonacci << ","
                << (allValid ? "YES" : "NO") << "\n" << std::flush;
    }
}

int main() {
    std::cout << "=== Tarea 1: Algoritmo de Prim y Análisis Amortizado ===" << std::endl;
    std::cout << "Iniciando batería de experimentos...\n" << std::endl;
    
    std::ofstream outfile("results_experimental.csv");
    if (outfile.is_open()) {
        outfile << "Serie,i,j,V,E,TimeBinomial,TimeFibonacci,OpsBinomial,OpsFibonacci,Valid\n";
    } else {
        std::cerr << "Error: No se pudo crear el archivo de resultados CSV." << std::endl;
    }

    // ---------------------------------------------------------
    // 6.3.1. Costo total
    // ---------------------------------------------------------

    // En debug usamos grafos más pequeños para testear más rápido.
#ifdef _GLIBCXX_DEBUG
    // Serie A: v fijo pequeño, variando e
    std::cout << "--- Iniciando Serie A ---" << std::endl;
    int i_A = 12;
    std::vector<int> j_A = {12, 13, 14};
    for (int j : j_A) {
        ejecutarConfiguracion(i_A, j, "Serie A", outfile);
    }

    // Serie B: e fijo, variando v
    std::cout << "--- Iniciando Serie B ---" << std::endl;
    int j_B = 14;
    std::vector<int> i_B = {10, 11, 12, 13, 14};
    for (int i : i_B) {
        ejecutarConfiguracion(i, j_B, "Serie B", outfile);
    }

    // ---------------------------------------------------------
    // 6.3.2. Costo amortizado
    // ---------------------------------------------------------

    // Serie C: v fijo pequeño, variando e
    std::cout << "--- Iniciando Serie C ---" << std::endl;
    int i_C = 10;
    std::vector<int> j_C = {10, 11, 12};
    for (int j : j_C) {
        ejecutarConfiguracion(i_C, j, "Serie C", outfile);
    }

    // Serie D: e fijo, variando v
    std::cout << "--- Iniciando Serie D ---" << std::endl;
    int j_D = 12;
    std::vector<int> i_D = {8, 9, 10, 11, 12};
    for (int i : i_D) {
        ejecutarConfiguracion(i, j_D, "Serie D", outfile);
    }

#else
    // Serie A: v fijo (i=20), variando e (j entre 20 y 24)
    std::cout << "--- Iniciando Serie A ---" << std::endl;
    int i_A = 20;
    std::vector<int> j_A = {20, 21, 22, 23, 24};
    for (int j : j_A) {
        ejecutarConfiguracion(i_A, j, "Serie A", outfile);
    }

    // Serie B: e fijo (j=24), variando v (i entre 18 y 22)
    std::cout << "--- Iniciando Serie B ---" << std::endl;
    int j_B = 24;
    std::vector<int> i_B = {18, 19, 20, 21, 22};
    for (int i : i_B) {
        ejecutarConfiguracion(i, j_B, "Serie B", outfile);
    }

    // ---------------------------------------------------------
    // 6.3.2. Costo amortizado
    // ---------------------------------------------------------

    // Serie C: v fijo (i=18), variando e (j entre 18 y 22)
    std::cout << "--- Iniciando Serie C ---" << std::endl;
    int i_C = 18;
    std::vector<int> j_C = {18, 19, 20, 21, 22};
    for (int j : j_C) {
        ejecutarConfiguracion(i_C, j, "Serie C", outfile);
    }

    // Serie D: e fijo (j=22), variando v (i entre 14 y 18)
    std::cout << "--- Iniciando Serie D ---" << std::endl;
    int j_D = 22;
    std::vector<int> i_D = {14, 15, 16, 17, 18};
    for (int i : i_D) {
        ejecutarConfiguracion(i, j_D, "Serie D", outfile);
    }
#endif

    if (outfile.is_open()) {
        outfile.close();
        std::cout << "Todos los resultados fueron guardados en 'results_experimental.csv'" << std::endl;
    }

    return 0;
}