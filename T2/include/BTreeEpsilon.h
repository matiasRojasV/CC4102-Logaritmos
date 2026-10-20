#pragma once

#include "Common.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <limits>
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
        if (!(alpha_ > 0.0 && alpha_ < 1.0))
            throw std::invalid_argument("alpha debe estar estrictamente entre 0 y 1");
        nodes_.emplace_back();
    }

    void insert(Pair value) {
        ++counters_.reads;
        if (!nodes_[0].isInternal) {
            if (nodes_[0].bufferCount == static_cast<int>(LMAX))
                splitRootLeaf();
        } else if (nodes_[0].bufferCount == static_cast<int>(LMAX)) {
            flush(0);
            if (nodes_[0].bufferCount == static_cast<int>(LMAX))
                flush(0);
        }
        insertSorted(nodes_[0], value);
        ++counters_.writes;
    }

    Results rangeSearch(std::int32_t lower, std::int32_t upper,
                        const std::string& filename) {
        if (lower > upper) std::swap(lower, upper);
        std::ifstream file(filename, std::ios::binary);
        if (!file)
            throw std::runtime_error("No se pudo abrir el árbol B epsilon: " + filename);
        searchReads_ = 0;
        Results result;
        searchNode(file, 0, lower, upper, result);
        std::stable_sort(result.begin(), result.end(),
                         [](const Pair& a, const Pair& b) { return a.first < b.first; });
        return result;
    }

    void writeToDisk(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file)
            throw std::runtime_error("No se pudo crear el árbol B epsilon: " + filename);
        for (const auto& node : nodes_)
            file.write(reinterpret_cast<const char*>(&node), sizeof(node));
        if (!file)
            throw std::runtime_error("Error escribiendo el árbol B epsilon");
    }

    std::uint64_t insertionReads() const { return counters_.reads; }
    std::uint64_t insertionWrites() const { return counters_.writes; }
    std::uint64_t searchReads() const { return searchReads_; }
    std::size_t size() const { return nodes_.size(); }
    void resetCounters() { counters_.reset(); searchReads_ = 0; }

private:
    using Node = BTreeEpsilonNode<C, LMAX>;

    double alpha_;
    std::vector<Node> nodes_;
    Counters counters_;
    std::uint64_t searchReads_ = 0;

    static void insertSorted(Node& node, Pair value) {
        int position = node.bufferCount;
        while (position > 0 && node.buffer[position - 1].first > value.first) {
            node.buffer[position] = node.buffer[position - 1];
            --position;
        }
        node.buffer[position] = value;
        ++node.bufferCount;
    }

    static void insertSorted(Node& node, const std::vector<Pair>& values) {
        for (const Pair& value : values) insertSorted(node, value);
    }

    int parentOf(int child) const {
        for (int i = 0; i < static_cast<int>(nodes_.size()); ++i)
            if (i != child)
            for (int j = 0; j <= nodes_[i].keyCount; ++j)
                if (nodes_[i].children[j] == child && nodes_[i].isInternal)
                    return i;
        return -1;
    }

    int childPosition(const Node& parent, std::int32_t key) const {
        int position = 0;
        while (position < parent.keyCount && key > parent.keys[position]) ++position;
        return position;
    }

    int splitLeaf(int leafIndex) {
        Node& leaf = nodes_[leafIndex];
        Node right;
        const int middle = static_cast<int>(LMAX / 2);
        right.bufferCount = leaf.bufferCount - middle;
        for (int i = 0; i < right.bufferCount; ++i)
            right.buffer[i] = leaf.buffer[middle + i];
        leaf.bufferCount = middle;
        nodes_.push_back(right);
        ++counters_.writes;
        return static_cast<int>(nodes_.size() - 1);
    }

    void splitRootLeaf() {
        const int oldLeaf = static_cast<int>(nodes_.size());
        nodes_.push_back(nodes_[0]);
        const int right = splitLeaf(oldLeaf);
        Node root;
        root.isInternal = 1;
        root.keyCount = 1;
        root.keys[0] = nodes_[oldLeaf].buffer[nodes_[oldLeaf].bufferCount - 1].first;
        root.children[0] = oldLeaf;
        root.children[1] = right;
        nodes_[0] = root;
        ++counters_.writes;
    }

    int splitInternal(int index) {
        Node& node = nodes_[index];
        Node right;
        right.isInternal = 1;
        const int middle = node.keyCount / 2;
        const int separator = node.keys[middle];
        const int oldBufferCount = node.bufferCount;
        const std::array<Pair, LMAX> oldBuffer = node.buffer;
        right.keyCount = node.keyCount - middle - 1;
        for (int i = 0; i < right.keyCount; ++i)
            right.keys[i] = node.keys[middle + 1 + i];
        for (int i = 0; i <= right.keyCount; ++i)
            right.children[i] = node.children[middle + 1 + i];
        node.bufferCount = 0;
        for (int i = 0; i < oldBufferCount; ++i) {
            if (oldBuffer[i].first <= separator)
                insertSorted(node, oldBuffer[i]);
            else
                insertSorted(right, oldBuffer[i]);
        }
        node.keyCount = middle;
        nodes_.push_back(right);
        ++counters_.writes;
        return static_cast<int>(nodes_.size() - 1);
    }

    void addChild(int parentIndex, int position, std::int32_t separator, int rightChild) {
        if (nodes_[parentIndex].keyCount < static_cast<int>(C)) {
            for (int i = nodes_[parentIndex].keyCount; i > position; --i) {
                nodes_[parentIndex].keys[i] = nodes_[parentIndex].keys[i - 1];
                nodes_[parentIndex].children[i + 1] = nodes_[parentIndex].children[i];
            }
            nodes_[parentIndex].keys[position] = separator;
            nodes_[parentIndex].children[position + 1] = rightChild;
            ++nodes_[parentIndex].keyCount;
            return;
        }

        const int grand = parentOf(parentIndex);
        if (grand < 0) {
            const int leftParent = static_cast<int>(nodes_.size());
            Node parentCopy = nodes_[parentIndex];
            nodes_.push_back(parentCopy);
            
            const std::int32_t middleKey = nodes_[leftParent].keys[nodes_[leftParent].keyCount / 2];
            const int rightParent = splitInternal(leftParent);
            
            Node newRoot;
            newRoot.isInternal = 1;
            newRoot.keyCount = 1;
            newRoot.keys[0] = middleKey;
            newRoot.children[0] = leftParent;
            newRoot.children[1] = rightParent;
            nodes_[0] = newRoot;
            
            // CORRECCIÓN CRÍTICA: Comparar contra middleKey
            parentIndex = separator < middleKey ? leftParent : rightParent;
        } else {
            const std::int32_t middleKey = nodes_[parentIndex].keys[nodes_[parentIndex].keyCount / 2];
            const int rightParent = splitInternal(parentIndex);
            const int parentPosition = childPosition(nodes_[grand], middleKey);
            
            addChild(grand, parentPosition, middleKey, rightParent);
            
            // CORRECCIÓN CRÍTICA: Comparar contra middleKey
            parentIndex = separator < middleKey ? parentIndex : rightParent;
        }
        
        const int newPosition = childPosition(nodes_[parentIndex], separator);
        addChild(parentIndex, newPosition, separator, rightChild);
    }
    

    void deliver(int parentIndex, std::vector<Pair> values) {
        if (values.empty()) return;
        
        int position = childPosition(nodes_[parentIndex], values.front().first);
        int childIndex = nodes_[parentIndex].children[position];
        
        // Se accede directamente mediante nodes_[childIndex] en lugar de crear una referencia
        if (nodes_[childIndex].bufferCount + static_cast<int>(values.size()) <= static_cast<int>(LMAX)) {
            insertSorted(nodes_[childIndex], values);
            ++counters_.writes;
            return;
        }

        if (!nodes_[childIndex].isInternal) {
            const int right = splitLeaf(childIndex);
            const std::int32_t separator =
                nodes_[childIndex].buffer[nodes_[childIndex].bufferCount - 1].first;
            addChild(parentIndex, position, separator, right);
            deliver(parentIndex, std::move(values));
            return;
        }

        flush(childIndex);
        
        if (nodes_[childIndex].keyCount == static_cast<int>(C)) {
            const std::int32_t separator = nodes_[childIndex].keys[nodes_[childIndex].keyCount / 2];
            const int right = splitInternal(childIndex);
            addChild(parentIndex, position, separator, right);
        }
        deliver(parentIndex, std::move(values));
    }

    void flush(int parentIndex) {
        if (!nodes_[parentIndex].isInternal || nodes_[parentIndex].bufferCount == 0) return;
        
        const int numChildren = nodes_[parentIndex].keyCount + 1;
        std::vector<std::vector<Pair>> groups(numChildren);
        
        for (int i = 0; i < nodes_[parentIndex].bufferCount; ++i) {
            int pos = childPosition(nodes_[parentIndex], nodes_[parentIndex].buffer[i].first);
            groups[pos].push_back(nodes_[parentIndex].buffer[i]);
        }

        const int f = static_cast<int>(
            std::ceil(static_cast<double>(nodes_[parentIndex].bufferCount) / numChildren));
        const int threshold = static_cast<int>(std::ceil(f * alpha_));
        std::vector<Pair> remaining;
        
        nodes_[parentIndex].bufferCount = 0;
        
        for (int i = numChildren - 1; i >= 0; --i) {
            if (static_cast<int>(groups[i].size()) >= threshold) {
                deliver(parentIndex, std::move(groups[i])); // Esto puede reasignar memoria
            } else {
                remaining.insert(remaining.end(), groups[i].begin(), groups[i].end());
            }
        }
        
        // Se accede de forma segura mediante el índice actualizado
        insertSorted(nodes_[parentIndex], remaining);
        ++counters_.writes;
    }

    void searchNode(std::ifstream& file, int index, std::int32_t lower,
                    std::int32_t upper, Results& result) {
        file.seekg(static_cast<std::streamoff>(index) * sizeof(Node));
        Node node;
        if (!file.read(reinterpret_cast<char*>(&node), sizeof(node)))
            throw std::runtime_error("Error leyendo el árbol B epsilon");
        ++searchReads_;
        for (int i = 0; i < node.bufferCount; ++i)
            if (inRange(node.buffer[i], lower, upper)) result.push_back(node.buffer[i]);
        if (!node.isInternal) return;
        for (int i = 0; i <= node.keyCount; ++i) {
            const std::int32_t left = i == 0 ? std::numeric_limits<std::int32_t>::min()
                                             : node.keys[i - 1];
            const std::int32_t right = i == node.keyCount ? std::numeric_limits<std::int32_t>::max()
                                                          : node.keys[i];
            if (left <= upper && right >= lower)
                searchNode(file, node.children[i], lower, upper, result);
        }
    }
};

static_assert(sizeof(BTreeEpsilonNode<4, 506>) == BLOCK_SIZE);
static_assert(sizeof(BTreeEpsilonNode<126, 384>) == BLOCK_SIZE);
