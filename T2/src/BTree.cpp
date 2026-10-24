#include "BTree.h"
#include <algorithm>
#include <stdexcept>


BTree::BTree() {
    BTreeNode root;
    nodes_.push_back(root);
    ++counters_.writes;
}

// Cada lectura o escritura del arreglo representa un acceso a un bloque.
BTreeNode BTree::readNode(int index) {
    ++counters_.reads;
    return nodes_[index];
}

void BTree::writeNode(int index, const BTreeNode& node) {
    nodes_[index] = node;
    ++counters_.writes;
}

// Guarda un nodo nuevo al final del arreglo en mem
int BTree::append(const BTreeNode& node) {
    nodes_.push_back(node);
    ++counters_.writes;
    return static_cast<int>(nodes_.size() - 1);
}


// Separa un nodo lleno en dos nodos y sube la llave mediana
void BTree::splitChild(int parent, int position) {
    const BTreeNode parentNode = readNode(parent);
    const int oldIndex = parentNode.children[position];
    const BTreeNode old = readNode(oldIndex);

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

    writeNode(oldIndex, left);

    const int rightIndex = append(right);

    BTreeNode p = parentNode;
    for (int i = p.count; i >= position + 1; --i) p.children[i + 1] = p.children[i];
    p.children[position + 1] = rightIndex;
    for (int i = p.count - 1; i >= position; --i) p.items[i + 1] = p.items[i];
    p.items[position] = median;
    ++p.count;
    writeNode(parent, p);
}


// Inserta el valor en una hoja o desciende al hijo correspondiente
void BTree::insertNonFull(int node, Pair value) {
    BTreeNode current = readNode(node);
    if (!current.isInternal) {
        int i = current.count - 1;
        while (i >= 0 && current.items[i].first > value.first) {
            current.items[i + 1] = current.items[i];
            --i;
        }
        current.items[i + 1] = value;
        ++current.count;
        writeNode(node, current);
        return;
    }

    int position = 0;
    while (position < current.count && value.first > current.items[position].first) {
        ++position;
    }

    const int child = current.children[position];
    if (readNode(child).count == static_cast<int>(BTREE_B)) {
        splitChild(node, position);
        current = readNode(node);
        if (value.first > current.items[position].first) {
            ++position;
        }
    }
    current = readNode(node);
    insertNonFull(current.children[position], value);
}


// Inserta un valor; si la raíz está llena, crea una nueva raíz
void BTree::insert(Pair value) {
    if (readNode(0).count == static_cast<int>(BTREE_B)) {
        BTreeNode oldRoot = readNode(0);
        const int oldIndex = append(oldRoot);

        BTreeNode newRoot;
        newRoot.isInternal = 1;
        newRoot.children[0] = oldIndex;
        writeNode(0, newRoot);

        splitChild(0, 0);
    }
    insertNonFull(0, value);
}


// Lee un nodo desde disco y explora los subárboles relevantes
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

    // Acotar busquedas descendentes únicamente a los subarboles que intersectan [lower, upper]
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


// Ejecuta una busqueda inclusiva y normaliza los limites si es necesario
Results BTree::rangeSearch(std::int32_t lower, std::int32_t upper, const std::string& filename) {
    if (lower > upper) std::swap(lower, upper);
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("No se pudo abrir el archivo: " + filename);
    
    searchReads_ = 0;
    Results result;
    searchNode(file, 0, lower, upper, result);
    std::stable_sort(result.begin(), result.end(),
                     [](const Pair& a, const Pair& b) {
                         return a.first < b.first;
                     });
    return result;
}


// Escribe los nodos en orden, usando un bloque por nodo
void BTree::writeToDisk(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file) throw std::runtime_error("No se pudo crear el archivo: " + filename);
    for (const BTreeNode& node : nodes_) {
        file.write(reinterpret_cast<const char*>(&node), sizeof(node));
    }
}