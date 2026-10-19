#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <iostream>

constexpr std::size_t BLOCK_SIZE = 4096;

#pragma pack(push, 1)
struct Pair {
    std::int32_t first; // Tiempo Unix (4 bytes)
    float second;       // Temperatura (4 bytes)
};
#pragma pack(pop)

static_assert(sizeof(Pair) == 8, "El tamaño de Pair debe ser exactamente 8 bytes");

using Results = std::vector<Pair>;

struct Counters {
    std::uint64_t reads = 0;
    std::uint64_t writes = 0;
    void reset() { reads = writes = 0; }
};

inline bool inRange(const Pair& p, std::int32_t lower, std::int32_t upper) {
    return p.first >= lower && p.first <= upper;
}