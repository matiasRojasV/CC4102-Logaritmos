#include "BTree.h"
#include <algorithm>
#include <stdexcept>

BTree::BTree() {
    BTreeNode root;
    nodes_.push_back(root);
    counters_.writes++;
}

int BTree::append(const BTreeNode& node) {
    nodes_.push_back(node);
    ++counters_.writes;
    return static_cast<int>(nodes_.size() - 1);
}

void BTree::splitChild(int parent, int position) {
    int oldIndex = nodes_[parent].children[position];
    BTreeNode old = nodes_[oldIndex];
    ++counters_.reads;

    BTreeNode left = old, right = old;
    const int middle = static_cast<int>(BTREE_B / 2) - 1; // Posición mediana
    const Pair median = old.items[middle];

    left.count = middle; // Elementos a la izquierda
    right.count = static_cast<int>(BTREE_B) - (middle + 1);

    for (int i = 0; i < right.count; ++i) {
        right.items[i] = old.items[middle + 1 + i];
    }

    if (old.isInternal) {
        left.isInternal = right.isInternal = 1;
        for (int i = 0; i <= left.count; ++i) left.children[i] = old.children[i];
        for (int i = 0; i <= right.count; ++i) {
            right.children[i] = old.children[middle + 1 + i];
        }
    }

    nodes_[oldIndex] = left;
    ++counters_.writes;

    const int rightIndex = append(right);

    // Re-obtener referencia tras el append para evitar dangling reference
    BTreeNode& p = nodes_[parent];
    for (int i = p.count; i >= position + 1; --i) p.children[i + 1] = p.children[i];
    p.children[position + 1] = rightIndex;
    for (int i = p.count - 1; i >= position; --i) p.items[i + 1] = p.items[i];
    p.items[position] = median;
    ++p.count;
    ++counters_.writes;
}

void BTree::insertNonFull(int node, Pair value) {
    ++counters_.reads;
    if (!nodes_[node].isInternal) {
        BTreeNode& current = nodes_[node];
        int i = current.count - 1;
        while (i >= 0 && current.items[i].first > value.first) {
            current.items[i + 1] = current.items[i];
            --i;
        }
        current.items[i + 1] = value;
        ++current.count;
        ++counters_.writes;
        return;
    }

    int position = 0;
    while (position < nodes_[node].count && value.first > nodes_[node].items[position].first) {
        ++position;
    }

    int child = nodes_[node].children[position];
    if (nodes_[child].count == static_cast<int>(BTREE_B)) {
        splitChild(node, position);
        if (value.first > nodes_[node].items[position].first) {
            ++position;
        }
    }
    insertNonFull(nodes_[node].children[position], value);
}

void BTree::insert(Pair value) {
    if (nodes_[0].count == static_cast<int>(BTREE_B)) {
        BTreeNode oldRoot = nodes_[0];
        const int oldIndex = append(oldRoot);

        BTreeNode newRoot;
        newRoot.isInternal = 1;
        newRoot.children[0] = oldIndex;
        nodes_[0] = newRoot;
        ++counters_.writes;

        splitChild(0, 0);
    }
    insertNonFull(0, value);
}

void BTree::searchNode(std::ifstream& file, int node, std::int32_t lower,
                       std::int32_t upper, Results& result) const {
    file.seekg(static_cast<std::streamoff>(node) * BLOCK_SIZE);
    BTreeNode current{};
    if (!file.read(reinterpret_cast<char*>(&current), sizeof(current))) {
        throw std::runtime_error("No se pudo leer un nodo del árbol B");
    }
    ++searchReads_;

    for (int i = 0; i < current.count; ++i) {
        if (inRange(current.items[i], lower, upper)) {
            result.push_back(current.items[i]);
        }
    }

    if (!current.isInternal) return;

    // Acotar búsquedas descendentes únicamente a los subárboles que intersectan [lower, upper]
    for (int i = 0; i <= current.count; ++i) {
        bool explore = false;
        if (i == 0) {
            if (lower <= current.items[0].first) explore = true;
        } else if (i == current.count) {
            if (upper > current.items[current.count - 1].first) explore = true;
        } else {
            if (lower <= current.items[i].first && upper > current.items[i - 1].first) explore = true;
        }

        if (explore) {
            searchNode(file, current.children[i], lower, upper, result);
        }
    }
}

Results BTree::rangeSearch(std::int32_t lower, std::int32_t upper, const std::string& filename) {
    if (lower > upper) std::swap(lower, upper);
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("No se pudo abrir el archivo: " + filename);
    
    searchReads_ = 0;
    Results result;
    searchNode(file, 0, lower, upper, result);
    return result;
}

void BTree::writeToDisk(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file) throw std::runtime_error("No se pudo crear el archivo: " + filename);
    for (const BTreeNode& node : nodes_) {
        file.write(reinterpret_cast<const char*>(&node), sizeof(node));
    }
}