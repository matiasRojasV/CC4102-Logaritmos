#include "BinomialHeap.h"
#include <stdexcept>


BinomialHeap::BinomialHeap() : head(nullptr), size(0) {}

BinomialHeap::BinomialHeap(const std::vector<std::pair<int, double>>& priorities) 
    : head(nullptr), size(0) {
    nodeMap.resize(priorities.size());
    for (const auto& [key, priority] : priorities) {
        insert(key, priority);
    }
}

BinomialHeap::~BinomialHeap() {
    deleteAll(head);
}

void BinomialHeap::deleteAll(Node* node) {
    while (node) {
        Node* next = node->sibling;
        deleteAll(node->child);
        delete node;
        node = next;
    }
}

BinomialHeap::Node* BinomialHeap::linkTrees(Node* tree1, Node* tree2) {
    if (tree2->priority < tree1->priority) {
        std::swap(tree1, tree2);
    }
    
    tree2->parent = tree1;
    tree2->sibling = tree1->child;
    tree1->child = tree2;
    tree1->degree++;
    
    return tree1;
}

BinomialHeap::Node* BinomialHeap::merge(Node* h1, Node* h2) {
    if (!h1) return h2;
    if (!h2) return h1;
    
    Node* result = nullptr;
    Node** current = &result;
    
    while (h1 && h2) {
        if (h1->degree <= h2->degree) {
            *current = h1;
            h1 = h1->sibling;
        } else {
            *current = h2;
            h2 = h2->sibling;
        }
        current = &(*current)->sibling;
    }
    
    *current = h1 ? h1 : h2;
    return result;
}

void BinomialHeap::consolidate() {
    if (!head) return;
    
    int maxDegree = 64;
    std::vector<Node*> degreeTable(maxDegree, nullptr);
    
    Node* current = head;
    head = nullptr;
    Node** headPtr = &head;
    
    while (current) {
        Node* next = current->sibling;
        current->sibling = nullptr;
        current->parent = nullptr;
        
        int degree = current->degree;
        while (degree < maxDegree && degreeTable[degree]) {
            current = linkTrees(current, degreeTable[degree]);
            degreeTable[degree] = nullptr;
            degree++;
        }
        
        degreeTable[degree] = current;
        current = next;
    }
    
    // Reconstruir la lista de raíces
    for (int i = 0; i < maxDegree; i++) {
        if (degreeTable[i]) {
            degreeTable[i]->sibling = nullptr;
            *headPtr = degreeTable[i];
            headPtr = &degreeTable[i]->sibling;
        }
    }
}

void BinomialHeap::insert(int key, double priority) {
    Node* newNode = new Node(key, priority);
    
    newNode->parent = nullptr;
    newNode->child = nullptr;
    newNode->degree = 0;

    if (key >= (int)nodeMap.size()) {
        int newSize = std::max(key + 1, (int)nodeMap.size() * 2 + 1);
        nodeMap.resize(newSize, nullptr);
    }
    
    nodeMap[key] = newNode;

    newNode->sibling = head;
    head = newNode;
    size++;
}

int BinomialHeap::findMin() const {
    if (!head) return -1;
    
    int minKey = head->key;
    double minPriority = head->priority;
    
    Node* current = head->sibling;
    while (current) {
        if (current->priority < minPriority) {
            minKey = current->key;
            minPriority = current->priority;
        }
        current = current->sibling;
    }
    
    return minKey;
}

int BinomialHeap::extractMin() {
    if (!head) return -1;
    
    // Encontrar el nodo con mínima prioridad
    Node* minNode = head;
    Node* prev = nullptr;
    Node* current = head;
    double minPriority = head->priority;
    
    while (current->sibling) {
        if (current->sibling->priority < minPriority) {
            minPriority = current->sibling->priority;
            minNode = current->sibling;
            prev = current;
        }
        current = current->sibling;
    }
    
    // Desenlazar minNode de la lista de raíces
    if (prev) {
        prev->sibling = minNode->sibling;
    } else {
        head = minNode->sibling;
    }
    
    // Merge de los hijos de minNode con la lista restante
    Node* childList = minNode->child;
    // Invertir la lista dparae hijos
    Node* reversed = nullptr;
    while (childList) {
        Node* next = childList->sibling;
        childList->sibling = reversed;
        childList->parent = nullptr;
        reversed = childList;
        childList = next;
    }
    
    head = merge(head, reversed);
    
    int key = minNode->key;
    if (key < (int)nodeMap.size()) {
        nodeMap[key] = nullptr;
    }
    delete minNode;
    size--;
    
    consolidate();
    
    return key;
}

void BinomialHeap::decreaseKey(int key, double newPriority) {
    if (key < 0 || key >= (int)nodeMap.size() || !nodeMap[key]) {
        return; 
    }
    
    Node* node = nodeMap[key];
    
    if (newPriority >= node->priority) {
        return; // Ignorar si la prioridad no mejora
    }
    
    node->priority = newPriority;
    int swaps = 0;
    
    // Bubble up si es necesario
    while (node->parent && node->priority < node->parent->priority) {
        std::swap(node->key, node->parent->key);
        std::swap(node->priority, node->parent->priority);
        
        nodeMap[node->key] = node;
        nodeMap[node->parent->key] = node->parent;
        
        node = node->parent;
        swaps++; 
    }
    swapsHistory.push_back(swaps);
}

int BinomialHeap::getSize() const {
    return size;
}

bool BinomialHeap::isEmpty() const {
    return size == 0;
}
