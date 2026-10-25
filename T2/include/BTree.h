#pragma once
#include "BTreeNode.h"
#include <fstream>
#include <string>
#include <vector>

class BTree {
public:
    // Crea un árbol con una raíz vacía y la deja en la posición 0
    BTree();

    // Inserta value y mantiene ordenadas las llaves de cada nodo
    // Parámetro: value, par llave-temperatura que se incorporará al árbol
    void insert(Pair value);

    // Lee desde filename los pares con llave en el intervalo inclusivo [lower, upper] y los retorna
    Results rangeSearch(std::int32_t lower, std::int32_t upper, const std::string& filename);

    // Serializa todos los nodos, un bloque de 4096 bytes por nodo, en filename
    void writeToDisk(const std::string& filename) const;
    
    // Métricas acumuladas de inserción y de la última búsqueda ejecutada
    std::uint64_t insertionReads() const { return counters_.reads; }
    std::uint64_t insertionWrites() const { return counters_.writes; }
    std::uint64_t searchReads() const { return searchReads_; }
    std::size_t size() const { return nodes_.size(); }
    void resetCounters() { counters_.reset(); searchReads_ = 0; }


private:
    std::vector<BTreeNode> nodes_;
    Counters counters_;
    mutable std::uint64_t searchReads_ = 0;

    // Accesos a nodos del arreglo que simula el disco en RAM
    BTreeNode readNode(int index);
    void writeNode(int index, const BTreeNode& node);

    // Agrega un nodo al arreglo simulado y retorna su índice
    int append(const BTreeNode& node);

    // Divide el hijo lleno indicado y actualiza el nodo padre
    void splitChild(int parent, int position);

    // Inserta recursivamente value en un nodo que no está lleno
    void insertNonFull(int node, Pair value);

    // Lee desde disco los nodos que pueden intersectar el rango
    void searchNode(std::ifstream& file, int node, std::int32_t lower, std::int32_t upper, Results& result) const;}; 