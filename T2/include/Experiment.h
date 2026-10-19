#pragma once
#include "Common.h"
#include <string>
#include <vector>

std::vector<Pair> readBinaryData(const std::string& filename, std::size_t limit);
void runExperiments(const std::string& dataFile, const std::string& outputDir);
void runAlphaBonus(const std::string& dataFile, const std::string& outputDir);