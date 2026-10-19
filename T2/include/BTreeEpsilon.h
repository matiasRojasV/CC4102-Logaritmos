#pragma once

#include "Common.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

template <std::size_t C, std::size_t LMAX>
struct BTreeEpsilonNode {
    std::int32_t isInternal = 0;
    std::int32_t keyCount = 0;
    std::int32_t bufferCount = 0;
    std::array<std::int32_t, C> keys{};
    std::array<std::int32_t, C + 1> children{};
    std::array<Pair, LMAX> buffer{};
};

template <std::size_t C, std::size_t LMAX>
class BTreeEpsilon {
public:
    explicit BTreeEpsilon(double alpha = 0.5) : alpha_(alpha) {
        nodes_.emplace_back();
    }

    void insert(Pair value) {
        ++counters_.reads;
        auto* leaf = &nodes_.back();
        int position = leaf->bufferCount;
        if (position == static_cast<int>(LMAX)) {
            BTreeEpsilonNode<C, LMAX> right;
            const int middle = static_cast<int>(LMAX / 2);
            right.bufferCount = leaf->bufferCount - middle;
            for (int i = 0; i < right.bufferCount; ++i)
                right.buffer[i] = leaf->buffer[middle + i];
            leaf->bufferCount = middle;
            nodes_.push_back(right);
            ++counters_.writes;
            leaf = &nodes_.back();
            position = 0;
        }
        while (position > 0 && leaf->buffer[position - 1].first > value.first) {
            leaf->buffer[position] = leaf->buffer[position - 1];
            --position;
        }
        leaf->buffer[position] = value;
        ++leaf->bufferCount;
        ++counters_.writes;
    }

    Results rangeSearch(std::int32_t lower, std::int32_t upper,
                        const std::string& filename) {
        if (lower > upper) std::swap(lower, upper);
        std::ifstream file(filename, std::ios::binary);
        if (!file) throw std::runtime_error("No se pudo abrir el árbol B epsilon: " + filename);
        searchReads_ = 0;
        Results result;
        BTreeEpsilonNode<C, LMAX> node;
        while (file.read(reinterpret_cast<char*>(&node), sizeof(node))) {
            ++searchReads_;
            for (int i = 0; i < node.bufferCount; ++i)
                if (inRange(node.buffer[i], lower, upper)) result.push_back(node.buffer[i]);
        }
        std::stable_sort(result.begin(), result.end(),
                         [](const Pair& a, const Pair& b) { return a.first < b.first; });
        return result;
    }

    void writeToDisk(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file) throw std::runtime_error("No se pudo crear el árbol B epsilon: " + filename);
        for (const auto& node : nodes_)
            file.write(reinterpret_cast<const char*>(&node), sizeof(node));
        if (!file) throw std::runtime_error("Error escribiendo el árbol B epsilon");
    }

    std::uint64_t insertionReads() const { return counters_.reads; }
    std::uint64_t insertionWrites() const { return counters_.writes; }
    std::uint64_t searchReads() const { return searchReads_; }
    std::size_t size() const { return nodes_.size(); }

private:
    double alpha_;
    std::vector<BTreeEpsilonNode<C, LMAX>> nodes_;
    Counters counters_;
    std::uint64_t searchReads_ = 0;
};

static_assert(sizeof(BTreeEpsilonNode<4, 506>) == BLOCK_SIZE);
static_assert(sizeof(BTreeEpsilonNode<126, 384>) == BLOCK_SIZE);
