#pragma once
#include "Common.h"
#include <array>

constexpr std::size_t BTREE_B = 340;

// Nodo de un arbol B su tamaño coincide con un bloque de disco
#pragma pack(push, 1)

struct BTreeNode {
    std::int32_t isInternal = 0;               // 4 bytes
    std::int32_t count = 0;                    // 4 bytes
    std::array<Pair, BTREE_B> items{};         // 340 * 8 = 2720 bytes
    std::array<std::int32_t, BTREE_B + 1> children{}; // 341 * 4 = 1364 bytes
    std::int32_t padding = 0;                  // 4 bytes
};

#pragma pack(pop)

static_assert(sizeof(BTreeNode) == BLOCK_SIZE, "El nodo del Arbol B debe ocupar exactamente 4096 bytes");