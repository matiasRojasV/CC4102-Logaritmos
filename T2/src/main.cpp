#include "Experiment.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <datos.bin> [directorio_salida]\n";
        return 1;
    }

    std::string dataFile = argv[1];
    std::string outputDir = argc >= 3 ? argv[2] : ".";

    try {
        std::cout << "Iniciando experimentos Tarea 2...\n";
        runExperiments(dataFile, outputDir);
        std::cout << "Iniciando experimento adicional de alpha...\n";
        runAlphaBonus(dataFile, outputDir);
        std::cout << "Experimentos completados exitosamente.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}