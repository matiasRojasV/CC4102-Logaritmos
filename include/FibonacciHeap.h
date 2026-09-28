#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <vector>
#include <cmath>
#include <memory>

/**
 * @class FibonacciHeap
 * @brief Implementacion de una cola de Fibonacci de minimo.
 * Estructura avanzada que permite operaciones de decreaseKey en tiempo
 * O(1) amortizado y extractMin en tiempo O(log n) amortizado.
 */
class FibonacciHeap {
private:
    struct Node {
        int key;           // Valor de la clave 
        double priority;   // Valor de prioridad
        int degree;        // Numero de hijos
        bool marked;       // Flag para cascading cuts
        Node* parent;      // Puntero al padre
        Node* child;       // Puntero a un hijo 
        Node* left;        // Hermano izquierdo 
        Node* right;       // Hermano derecho
        
        Node(int k, double p)
            : key(k), priority(p), degree(0), marked(false),
              parent(nullptr), child(nullptr), left(this), right(this) {}
    };
    

    Node* minNode;  // Puntero al nodo min
    int size;       // Numero de elementos
    int currentCuts = 0;

    // Mapa para acceso rápido a nodos por clave
    std::vector<Node*> nodeMap;
    
    /**
     * Inserta un nodo en la lista de raíces
     */
    void insertIntoRootList(Node* node);
    
    /**
     * Elimina un nodo de la lista doblemente enlazada circular
     */
    void removeFromList(Node* node);
    
    /**
     * Linkea dos arboles de Fibonacci
     */
    void link(Node* child, Node* parent);
    
    /**
     * Consolida los arboles en la estructura
     */
    void consolidate();
    
    /**
     * Corta un nodo de su padre
     */
    void cut(Node* node);
    
    /**
     * Cortes en cascada de un nodo
     */
    void cascadingCut(Node* node);
    
    /**
     * Libera toda la memoria del heap
     */
    void deleteAll(Node* node);
    
public:
    std::vector<int> cutsHistory;
    std::vector<long long> timeHistory;
    
    /**
     * Constructor que crea una cola de Fibonacci vacia
     */
    FibonacciHeap();
    
    /**
     * Constructor que inicializa el heap con un vector de prioridades
     * @param priorities Vector de pares
     */
    FibonacciHeap(const std::vector<std::pair<int, double>>& priorities);
    
    /**
     * Destructor
     */
    ~FibonacciHeap();
    
    /**
     * Inserta un elemento con clave y prioridad
     * @param key Identificador del elemento
     * @param priority Valor de prioridad
     * Tiempo: O(1) amortizado
     */
    void insert(int key, double priority);
    
    /**
     * Retorna la clave con min prioridad sin extraerla
     * @return Clave del min, o -1 si esta vacio
     * Tiempo: O(1)
     */
    int findMin() const;
    
    /**
     * Extrae y retorna la clave con min prioridad
     * @return Clave del min, o -1 si esta vacio
     * Tiempo: O(log n) amortizado
     */
    int extractMin();
    
    /**
     * Disminuye la prioridad de un elemento
     * @param key Identificador del elemento
     * @param newPriority Nueva prioridad
     * Tiempo: O(1) amortizado
     */
    void decreaseKey(int key, double newPriority);
    
    /**
     * Retorna el tamaño del heap
     */
    int getSize() const;
    
    /**
     * Verifica si el heap está vacio
     */
    bool isEmpty() const;
};

#endif
