#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

#include <vector>
#include <limits>
#include <memory>

/**
 * @class BinomialHeap
 * @brief Implementación de una cola binomial de mínimo
 * 
 * Estructura de datos que mantiene un mínimo en tiempo O(1) amortizado
 * y permite operaciones de extractMin en O(log n)
 */
class BinomialHeap {
private:
    struct Node {
        int key;           // Valor de la clave
        double priority;   // Valor de prioridades
        int degree;        // Grado del nodo
        Node* parent;      // Puntero al padre
        Node* child;       // Puntero al hijo más a la izquierda
        Node* sibling;     // Puntero al hermano derecho
        
        Node(int k, double p) 
            : key(k), priority(p), degree(0), parent(nullptr), 
              child(nullptr), sibling(nullptr) {}
    };
    
    Node* head;  // Cabeza de la lista de raices
    int size;    // Numero de elementos
    
    // Mapa para acceso rapido a nodos por clave
    std::vector<Node*> nodeMap;
    
    /**
     * Linkea dos árboles binomiales
     */
    Node* linkTrees(Node* tree1, Node* tree2);
    
    /**
     * Merge de dos listas de raices
     */
    Node* merge(Node* h1, Node* h2);
    
    /**
     * Consolida la estructura despues de operaciones
     */
    void consolidate();
    
    /**
     * Libera toda la memoria del heap
     */
    void deleteAll(Node* node);
    
public:
    std::vector<int> swapsHistory;
    std::vector<long long> timeHistory;
    /**
     * Constructor que crea una cola binomial vacia
     */
    BinomialHeap();
    
    /**
     * Constructor que inicializa el heap con un vector de prioridades
     * @param priorities Vector de pares (clave, prioridad)
     */
    BinomialHeap(const std::vector<std::pair<int, double>>& priorities);
    
    /**
     * Destructor
     */
    ~BinomialHeap();
    
    /**
     * Inserta un elemento con clave y prioridad
     * 
     * @param key Identificador del elemento
     * @param priority Valor de prioridad
     * Tiempo: O(log n) amortizado
     */
    void insert(int key, double priority);
    
    /**
     * Retorna la clave con min prioridad sin extraerla
     * @return Clave del minimo, o -1 si está vacío
     * Tiempo: O(1) amortizado
     */
    int findMin() const;
    
    /**
     * Extrae y retorna la clave con min prioridad
     * @return Clave del minimo, o -1 si este vacio
     * Tiempo: O(log n)
     */
    int extractMin();
    
    /**
     * Disminuye la prioridad de un elemento
     * @param key Identificador del elemento
     * @param newPriority Nueva prioridad
     * Tiempo: O(log n)
     */
    void decreaseKey(int key, double newPriority);
    
    /**
     * Retorna el tamaño del heap
     */
    int getSize() const;
    
    /**
     * Verifica si el heap esta vacio
     */
    bool isEmpty() const;
};

#endif