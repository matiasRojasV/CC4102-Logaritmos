#include "FibonacciHeap.h"
#include <algorithm>
#include <stdexcept>

FibonacciHeap::FibonacciHeap() : minNode(nullptr), size(0) {}

FibonacciHeap::FibonacciHeap(const std::vector<std::pair<int, double>>& priorities)
    : minNode(nullptr), size(0) {
    nodeMap.resize(priorities.size());
    for (const auto& [key, priority] : priorities) {
        insert(key, priority);
    }
}

FibonacciHeap::~FibonacciHeap() {
    if (minNode) {
        deleteAll(minNode);
    }
}

void FibonacciHeap::deleteAll(Node* node) {
    if (!node) return;
    
    Node* current = node;
    do {
        Node* next = current->right;
        deleteAll(current->child);
        delete current;
        current = next;
    } while (current != node);
}

void FibonacciHeap::insertIntoRootList(Node* node) {
    if (!minNode) {
        minNode = node;
        node->left = node;
        node->right = node;
    } else {
        node->left = minNode;
        node->right = minNode->right;
        minNode->right->left = node;
        minNode->right = node;
        
        if (node->priority < minNode->priority) {
            minNode = node;
        }
    }
}

void FibonacciHeap::removeFromList(Node* node) {
    if (node->left != node) {
        node->left->right = node->right;
        node->right->left = node->left;
    }
}

void FibonacciHeap::link(Node* child, Node* parent) {
    removeFromList(child);
    child->parent = parent;
    
    if (!parent->child) {
        parent->child = child;
        child->left = child;
        child->right = child;
    } else {
        child->left = parent->child;
        child->right = parent->child->right;
        parent->child->right->left = child;
        parent->child->right = child;
    }
    
    parent->degree++;
    child->marked = false;
}

void FibonacciHeap::consolidate() {
    if (!minNode) return;
    
    int maxDegree = static_cast<int>(2 * std::log2(size)) + 10;
    std::vector<Node*> degreeTable(maxDegree, nullptr);
    
    std::vector<Node*> roots;
    Node* current = minNode;
    do {
        roots.push_back(current);
        current = current->right;
    } while (current != minNode);
    
    for (Node* node : roots) {
        int degree = node->degree;
        
        while (degree < maxDegree && degreeTable[degree]) {
            Node* other = degreeTable[degree];
            
            if (other->priority < node->priority) {
                std::swap(node, other);
            }
            
            link(other, node);
            degreeTable[degree] = nullptr;
            degree++;
        }
        
        degreeTable[degree] = node;
    }
    
    minNode = nullptr;
    for (int i = 0; i < maxDegree; i++) {
        if (degreeTable[i]) {
            Node* node = degreeTable[i];
            node->left = node;
            node->right = node;
            insertIntoRootList(node);
        }
    }
}

void FibonacciHeap::insert(int key, double priority) {
    Node* newNode = new Node(key, priority);
    
    newNode->parent = nullptr;
    newNode->child = nullptr;
    newNode->degree = 0;
    newNode->marked = false;

    if (key >= (int)nodeMap.size()) {
        int newSize = std::max(key + 1, (int)nodeMap.size() * 2 + 1);
        nodeMap.resize(newSize, nullptr);
    }

    nodeMap[key] = newNode;
    insertIntoRootList(newNode);
    size++;
}

int FibonacciHeap::findMin() const {
    if (!minNode) return -1;
    return minNode->key;
}

int FibonacciHeap::extractMin() {
    if (!minNode) return -1;
    
    Node* oldMin = minNode;
    int key = oldMin->key;
    
    // Promover todos los hijos del mínimo a raíces de forma segura
    if (oldMin->child) {
        Node* child = oldMin->child;
        std::vector<Node*> children;
        
        // Guardamos los punteros primero para no romper el ciclo al modificar left/right
        Node* current = child;
        do {
            children.push_back(current);
            current = current->right;
        } while (current != child);
        
        // Los insertamos uno por uno en la lista de raíces
        for (Node* c : children) {
            c->parent = nullptr;
            c->left = c;
            c->right = c;
            insertIntoRootList(c);
        }
    }
    
    // Remover explícitamente a oldMin de la lista de raíces
    removeFromList(oldMin);
    
    // Ajustar minNode y consolidar
    if (oldMin == oldMin->right) {
        minNode = nullptr; // Era el único nodo en todo el heap
    } else {
        minNode = oldMin->right; // Apunte temporal, consolidate se encarga de hallar el verdadero mínimo
        consolidate();
    }
    
    size--;
    if (key < (int)nodeMap.size()) {
        nodeMap[key] = nullptr;
    }
    
    delete oldMin;
    return key;
}

void FibonacciHeap::cut(Node* node) {
    if (!node->parent) return;
    
    if (node->parent->child == node) {
        if (node->right != node) {
            node->parent->child = node->right;
        } else {
            node->parent->child = nullptr;
        }
    }
    
    removeFromList(node);
    node->parent->degree--;
    node->parent = nullptr;
    node->marked = false;
    
    insertIntoRootList(node);
    currentCuts++; // Contabilizar cada corte realizado (directo o en cascada)
}

void FibonacciHeap::cascadingCut(Node* node) {
    Node* parent = node->parent;
    
    if (parent) {
        if (!node->marked) {
            node->marked = true;
        } else {
            cut(node); // Al llamar a cut(), currentCuts se incrementará automáticamente
            cascadingCut(parent);
        }
    }
}

void FibonacciHeap::decreaseKey(int key, double newPriority) {
    // Escudo defensivo
    if (key < 0 || key >= (int)nodeMap.size() || !nodeMap[key]) {
        return; 
    }
    
    Node* node = nodeMap[key];
    
    if (newPriority >= node->priority) {
        return; // Ignorar si la prioridad no mejora
    }
    
    currentCuts = 0; 
    node->priority = newPriority;
    
    if (node->parent && node->priority < node->parent->priority) {
        Node* parent = node->parent;
        cut(node);
        cascadingCut(parent);
    }
    
    if (!minNode || node->priority < minNode->priority) {
        minNode = node;
    }
    
    cutsHistory.push_back(currentCuts);
}

int FibonacciHeap::getSize() const {
    return size;
}

bool FibonacciHeap::isEmpty() const {
    return size == 0;
}
