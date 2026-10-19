#include "Experiment.h"
#include "BTree.h"
#include "BTreeEpsilon.h"
#include <fstream>
#include <chrono>
#include <random>
#include <stdexcept>

std::vector<Pair> readBinaryData(const std::string& filename, std::size_t limit) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("No se pudo abrir el archivo de datos: " + filename);

    std::vector<Pair> values;
    values.reserve(limit);
    Pair value;
    while (values.size() < limit && file.read(reinterpret_cast<char*>(&value), sizeof(Pair))) {
        values.push_back(value);
    }
    return values;
}

void runExperiments(const std::string& dataFile, const std::string& outputDir) {
    std::ofstream csv(outputDir + "/experiment_results.csv");
    csv << "N,Tree,BuildTime,RAM_Reads,RAM_Writes,NumNodes,AvgSearchTime,AvgDiskReads\n";

    std::mt19937 rng(42);
    std::uniform_int_distribution<std::int32_t> dist(1546300800, 1754006400 - 604800); // Rango Unix Quinta Normal

    for (int exp = 15; exp <= 26; ++exp) {
        std::size_t N = 1ULL << exp;
        std::cout << "=== Ejecutando para N = 2^" << exp << " (" << N << ") ===" << std::endl;
        
        auto data = readBinaryData(dataFile, N);
        if (data.size() < N) break;

        // --- ARBOL B ---
        {
            BTree btree;
            auto t0 = std::chrono::high_resolution_clock::now();
            for (const auto& item : data) btree.insert(item);
            auto t1 = std::chrono::high_resolution_clock::now();
            double buildTime = std::chrono::duration<double>(t1 - t0).count();

            std::string path = outputDir + "/btree.bin";
            btree.writeToDisk(path);

            double totalSearchTime = 0;
            std::uint64_t totalReads = 0;

            for (int q = 0; q < 50; ++q) {
                std::int32_t l = dist(rng);
                std::int32_t u = l + 604800; // 1 semana
                
                auto st0 = std::chrono::high_resolution_clock::now();
                auto res = btree.rangeSearch(l, u, path);
                auto st1 = std::chrono::high_resolution_clock::now();

                totalSearchTime += std::chrono::duration<double>(st1 - st0).count();
                totalReads += btree.searchReads();
            }

            csv << N << ",BTree," << buildTime << "," << btree.insertionReads() << ","
                << btree.insertionWrites() << "," << btree.size() << ","
                << (totalSearchTime / 50.0) << "," << (totalReads / 50.0) << "\n";
        }

        // --- ARBOL B^eps (0.25) ---
        {
            BTreeEpsilon<4, 506> eps25(0.5);
            auto t0 = std::chrono::high_resolution_clock::now();
            for (const auto& item : data) eps25.insert(item);
            auto t1 = std::chrono::high_resolution_clock::now();
            double buildTime = std::chrono::duration<double>(t1 - t0).count();

            std::string path = outputDir + "/beps025.bin";
            eps25.writeToDisk(path);

            double totalSearchTime = 0;
            std::uint64_t totalReads = 0;

            for (int q = 0; q < 50; ++q) {
                std::int32_t l = dist(rng);
                std::int32_t u = l + 604800;
                
                auto st0 = std::chrono::high_resolution_clock::now();
                auto res = eps25.rangeSearch(l, u, path);
                auto st1 = std::chrono::high_resolution_clock::now();

                totalSearchTime += std::chrono::duration<double>(st1 - st0).count();
                totalReads += eps25.searchReads();
            }

            csv << N << ",Beps025," << buildTime << "," << eps25.insertionReads() << ","
                << eps25.insertionWrites() << "," << eps25.size() << ","
                << (totalSearchTime / 50.0) << "," << (totalReads / 50.0) << "\n";
        }
    }
    csv.close();
}