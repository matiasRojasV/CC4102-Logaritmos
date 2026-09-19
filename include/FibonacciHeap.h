#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <vector>
#include <cmath>
#include <memory>

/**
 * @class FibonacciHeap
 * @brief Implementación de una cola de Fibonacci de mínimo.
 * 
 * Estructura avanzada que permite operaciones de decreaseKey en tiempo
 * O(1) amortizado y extractMin en tiempo O(log n) amortizado.
 */
class FibonacciHeap {
private:
    struct Node {
        int key;           // Valor de la clave (identificador del vértice)
        double priority;   // Valor de prioridad (peso)
        int degree;        // Número de hijos
        bool marked;       // Flag para cascading cuts
        Node* parent;      // Puntero al padre
        Node* child;       // Puntero a un hijo (circular doubly-linked list)
        Node* left;        // Hermano izquierdo (circular doubly-linked list)
        Node* right;       // Hermano derecho (circular doubly-linked list)
        
        Node(int k, double p)
            : key(k), priority(p), degree(0), marked(false),
              parent(nullptr), child(nullptr), left(this), right(this) {}
    };
    
    Node* minNode;  // Puntero al nodo mínimo
    int size;       // Número de elementos
    
    // Mapa para acceso rápido a nodos por clave
    std::vector<Node*> nodeMap;
    
    /**
     * Inserta un nodo en la lista de raíces.
     */
    void insertIntoRootList(Node* node);
    
    /**
     * Elimina un nodo de la lista doblemente enlazada circular.
     */
    void removeFromList(Node* node);
    
    /**
     * Liga dos árboles de Fibonacci.
     */
    void link(Node* child, Node* parent);
    
    /**
     * Consolida los árboles en la estructura.
     */
    void consolidate();
    
    /**
     * Corta un nodo de su padre.
     */
    void cut(Node* node);
    
    /**
     * Cortes en cascada de un nodo.
     */
    void cascadingCut(Node* node);
    
    /**
     * Libera toda la memoria del heap.
     */
    void deleteAll(Node* node);
    
public:
    /**
     * Constructor que crea una cola de Fibonacci vacía.
     */
    FibonacciHeap();
    
    /**
     * Constructor que inicializa el heap con un vector de prioridades.
     * 
     * @param priorities Vector de pares (clave, prioridad)
     * 
     * Nota: Este constructor debe ser O(n) para heapify.
     */
    FibonacciHeap(const std::vector<std::pair<int, double>>& priorities);
    
    /**
     * Destructor.
     */
    ~FibonacciHeap();
    
    /**
     * Inserta un elemento con clave y prioridad.
     * 
     * @param key Identificador del elemento
     * @param priority Valor de prioridad
     * 
     * Tiempo: O(1) amortizado
     */
    void insert(int key, double priority);
    
    /**
     * Retorna la clave con mínima prioridad sin extraerla.
     * 
     * @return Clave del mínimo, o -1 si está vacío
     * 
     * Tiempo: O(1)
     */
    int findMin() const;
    
    /**
     * Extrae y retorna la clave con mínima prioridad.
     * 
     * @return Clave del mínimo, o -1 si está vacío
     * 
     * Tiempo: O(log n) amortizado
     */
    int extractMin();
    
    /**
     * Disminuye la prioridad de un elemento.
     * 
     * @param key Identificador del elemento
     * @param newPriority Nueva prioridad (debe ser menor)
     * 
     * Tiempo: O(1) amortizado
     */
    void decreaseKey(int key, double newPriority);
    
    /**
     * Retorna el tamaño del heap.
     */
    int getSize() const;
    
    /**
     * Verifica si el heap está vacío.
     */
    bool isEmpty() const;
};

#endif // FIBONACCI_HEAP_H
