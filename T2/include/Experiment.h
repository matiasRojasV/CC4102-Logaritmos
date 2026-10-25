#pragma once
#include "Common.h"
#include <string>
#include <vector>

// Lee hasta limit pares consecutivos desde un archivo binario,
// retorna menos elementos solo si el archivo termina antes de alcanzar limit
std::vector<Pair> readBinaryData(const std::string& filename, std::size_t limit);


// Construye los tres árboles solicitados, ejecuta 50 búsquedas por tamaño y
// guarda métricas en experiment_results.csv. También genera los datos de una
// búsqueda fija para N=2^15 y N=2^26 en range_comparison.csv
void runExperiments(const std::string& dataFile, const std::string& outputDir);


// Ejecuta el bonus con N=2^24, ocho valores de alpha y 20 búsquedas por valor
void runAlphaBonus(const std::string& dataFile, const std::string& outputDir);