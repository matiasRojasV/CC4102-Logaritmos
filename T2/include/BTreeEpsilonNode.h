#pragma once
#include "Common.h"
#include <array>

constexpr std::size_t EPSILON_MAX_C = 126;
constexpr std::size_t EPSILON_MAX_BUFFER = 506;

struct BTreeEpsilonNode {
    std::int32_t isInternal = 0, keyCount = 0, bufferCount = 0;
    std::array<std::int32_t, EPSILON_MAX_C> keys{};
    std::array<std::int32_t, EPSILON_MAX_C + 1> children{};
    std::array<Pair, EPSILON_MAX_BUFFER> buffer{};
};