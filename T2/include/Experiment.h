#pragma once
#include "Common.h"
#include <string>
#include <vector>

// Lee hasta limit pares desde un archivo binario
std::vector<Pair> readBinaryData(const std::string& filename, std::size_t limit);


// Construye los árboles y guarda las métricas principales en un CSV
void runExperiments(const std::string& dataFile, const std::string& outputDir);


// Ejecuta la comparación adicional usando distintos valores de alpha
void runAlphaBonus(const std::string& dataFile, const std::string& outputDir);