#include "Experiment.h"
#include "BTree.h"
#include "BTreeEpsilon.h"
#include <fstream>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <random>
#include <stdexcept>

namespace {
constexpr std::int32_t MIN_TIMESTAMP = 1546300800;
constexpr std::int32_t MAX_TIMESTAMP = 1754006400;
constexpr std::int32_t WEEK_SECONDS = 604800;
constexpr double MAIN_ALPHA = 0.5;

void ensureOutputDirectory(const std::string& outputDir) {
    std::error_code error;
    if (!std::filesystem::create_directories(outputDir, error) && error) {
        throw std::runtime_error("No se pudo crear el directorio de salida: " +
                                 outputDir + ": " + error.message());
    }
}

std::ofstream openOutput(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) throw std::runtime_error("No se pudo crear el archivo: " + filename);
    return file;
}

void writeRangeComparison(const std::string& filename, std::size_t n,
                          const Results& result) {
    std::ofstream file(filename, std::ios::app);
    if (!file) throw std::runtime_error("No se pudo abrir el archivo: " + filename);
    for (const Pair& value : result)
        file << n << "," << value.first << "," << value.second << "\n";
}
}  // namespace


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


// Construye los árboles, mide inserciones y busquedas, y guarda un CSV
void runExperiments(const std::string& dataFile, const std::string& outputDir) {
    ensureOutputDirectory(outputDir);
    const std::string comparisonPath = outputDir + "/range_comparison.csv";
    std::ofstream comparison(comparisonPath);
    if (!comparison)
        throw std::runtime_error("No se pudo crear el archivo: " + comparisonPath);
    comparison << "N,Key,Temperature\n";

    std::ofstream csv = openOutput(outputDir + "/experiment_results.csv");
    csv << "N,Tree,Alpha,BuildTime,RAM_Reads,RAM_Writes,NumNodes,"
           "AvgSearchTime,AvgDiskReads\n";

    std::mt19937 rng(42);
    std::uniform_int_distribution<std::int32_t> dist(
        MIN_TIMESTAMP, MAX_TIMESTAMP - WEEK_SECONDS);

    for (int exp = 15; exp <= 26; ++exp) {
        std::size_t N = 1ULL << exp;
        std::cout << "=== Ejecutando para N = 2^" << exp << " (" << N << ") ===" << std::endl;
        
        auto data = readBinaryData(dataFile, N);
        if (data.size() < N) break;


        // ARBOL B
        {
            std::cout << "  [BTree] construyendo..." << std::flush;
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
                std::int32_t u = l + WEEK_SECONDS; // 1 semana
                
                auto st0 = std::chrono::high_resolution_clock::now();
                auto res = btree.rangeSearch(l, u, path);
                auto st1 = std::chrono::high_resolution_clock::now();

                totalSearchTime += std::chrono::duration<double>(st1 - st0).count();
                totalReads += btree.searchReads();
            }

            if (exp == 15 || exp == 26) {
                const Results comparisonResult =
                    btree.rangeSearch(MIN_TIMESTAMP, MIN_TIMESTAMP + WEEK_SECONDS, path);
                comparison.flush();
                writeRangeComparison(comparisonPath, N, comparisonResult);
            }

            csv << N << ",BTree," << MAIN_ALPHA << "," << buildTime << ","
                << btree.insertionReads() << ","
                << btree.insertionWrites() << "," << btree.size() << ","
                << (totalSearchTime / 50.0) << "," << (totalReads / 50.0) << "\n";
            std::cout << " listo (" << btree.size() << " nodos)\n";
        }


        // ARBOL B^eps (0.25)
        {
            std::cout << "  [Beps025] construyendo..." << std::flush;
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
                std::int32_t u = l + WEEK_SECONDS;
                
                auto st0 = std::chrono::high_resolution_clock::now();
                auto res = eps25.rangeSearch(l, u, path);
                auto st1 = std::chrono::high_resolution_clock::now();

                totalSearchTime += std::chrono::duration<double>(st1 - st0).count();
                totalReads += eps25.searchReads();
            }

            csv << N << ",Beps025," << MAIN_ALPHA << "," << buildTime << ","
                << eps25.insertionReads() << ","
                << eps25.insertionWrites() << "," << eps25.size() << ","
                << (totalSearchTime / 50.0) << "," << (totalReads / 50.0) << "\n";
            std::cout << " listo (" << eps25.size() << " nodos)\n";
        }


        // ARBOL B^eps (0.75)
        {
            std::cout << "  [Beps075] construyendo..." << std::flush;
            BTreeEpsilon<126, 384> eps75(0.5);
            auto t0 = std::chrono::high_resolution_clock::now();
            for (const auto& item : data) eps75.insert(item);
            auto t1 = std::chrono::high_resolution_clock::now();
            double buildTime = std::chrono::duration<double>(t1 - t0).count();

            std::string path = outputDir + "/beps075.bin";
            eps75.writeToDisk(path);

            double totalSearchTime = 0;
            std::uint64_t totalReads = 0;

            for (int q = 0; q < 50; ++q) {
                std::int32_t l = dist(rng);
                std::int32_t u = l + WEEK_SECONDS;

                auto st0 = std::chrono::high_resolution_clock::now();
                auto res = eps75.rangeSearch(l, u, path);
                auto st1 = std::chrono::high_resolution_clock::now();

                totalSearchTime += std::chrono::duration<double>(st1 - st0).count();
                totalReads += eps75.searchReads();
            }

            csv << N << ",Beps075," << MAIN_ALPHA << "," << buildTime << ","
                << eps75.insertionReads() << ","
                << eps75.insertionWrites() << "," << eps75.size() << ","
                << (totalSearchTime / 50.0) << "," << (totalReads / 50.0) << "\n";
            std::cout << " listo (" << eps75.size() << " nodos)\n";
        }
    }
    csv.close();
    comparison.close();
}


// Repite el experimento con distintos valores del parametro alpha
void runAlphaBonus(const std::string& dataFile, const std::string& outputDir) {
    constexpr std::size_t N = 1ULL << 24;
    const auto data = readBinaryData(dataFile, N);
    if (data.size() < N)
        throw std::runtime_error("No hay suficientes datos para el experimento de alpha");

    ensureOutputDirectory(outputDir);
    std::ofstream csv = openOutput(outputDir + "/alpha_bonus_results.csv");
    csv << "N,Alpha,BuildTime,RAM_Reads,RAM_Writes,NumNodes,AvgSearchTime,AvgDiskReads\n";

    std::mt19937 rng(4242);
    std::uniform_int_distribution<std::int32_t> dist(
        1546300800, 1754006400 - 604800);

    for (int alphaIndex = 1; alphaIndex <= 8; ++alphaIndex) {
        const double alpha = alphaIndex / 10.0;
        BTreeEpsilon<4, 506> tree(alpha);

        auto t0 = std::chrono::high_resolution_clock::now();
        for (const auto& item : data) tree.insert(item);
        auto t1 = std::chrono::high_resolution_clock::now();
        const double buildTime = std::chrono::duration<double>(t1 - t0).count();

        const std::string path = outputDir + "/beps_alpha.bin";
        tree.writeToDisk(path);

        double totalSearchTime = 0;
        std::uint64_t totalReads = 0;
        for (int q = 0; q < 20; ++q) {
            const std::int32_t lower = dist(rng);
            const std::int32_t upper = lower + WEEK_SECONDS;

            auto st0 = std::chrono::high_resolution_clock::now();
            auto result = tree.rangeSearch(lower, upper, path);
            auto st1 = std::chrono::high_resolution_clock::now();

            totalSearchTime += std::chrono::duration<double>(st1 - st0).count();
            totalReads += tree.searchReads();
        }

        csv << N << "," << alpha << "," << buildTime << ","
            << tree.insertionReads() << "," << tree.insertionWrites() << ","
            << tree.size() << "," << (totalSearchTime / 20.0) << ","
            << (totalReads / 20.0) << "\n";
    }
}