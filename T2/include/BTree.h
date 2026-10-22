#pragma once
#include "BTreeNode.h"
#include <fstream>
#include <string>
#include <vector>

class BTree {
public:
    // Crea un arbol con una raiz vacia
    BTree();

    // Inserta un par llave-valor manteniendo el orden del arbol
    void insert(Pair value);

    // Busca en disco los pares cuyas llaves estan dentro del rango indicado
    Results rangeSearch(std::int32_t lower, std::int32_t upper, const std::string& filename);

    // Serializa todos los nodos del arbol en un archivo binario
    void writeToDisk(const std::string& filename) const;
    
    // Metricas de accesos simulados durante la inserción y la busqueda
    std::uint64_t insertionReads() const { return counters_.reads; }
    std::uint64_t insertionWrites() const { return counters_.writes; }
    std::uint64_t searchReads() const { return searchReads_; }
    std::size_t size() const { return nodes_.size(); }
    void resetCounters() { counters_.reset(); searchReads_ = 0; }


private:
    std::vector<BTreeNode> nodes_;
    Counters counters_;
    mutable std::uint64_t searchReads_ = 0;

    // Agrega un nodo al arreglo en RAM y devuelve su posición
    int append(const BTreeNode& node);

    // Divide un hijo lleno y actualiza su nodo padre
    void splitChild(int parent, int position);

    // Inserta recursivamente en un nodo que no está lleno
    void insertNonFull(int node, Pair value);

    // Recorre desde disco los nodos que pueden intersectar el rango
    void searchNode(std::ifstream& file, int node, std::int32_t lower, std::int32_t upper, Results& result) const;}; 