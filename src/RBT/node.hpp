#ifndef NODE_HPP
#define NODE_HPP
#include <utility>

#define BLACK '1'
#define RED '2'

template<typename K, typename V> class Map;
template<typename K, typename V> class Iterator;

template<typename K, typename V>
class Node {
public:
    char COLOR;
    std::pair<K, V> value;
    Node *l;
    Node *r;
    Node *p;

    Node(char COLOR = BLACK, std::pair<K, V> value = {K(), V()}, Node *l = nullptr, Node *r = nullptr, Node *p = nullptr) : 
                COLOR(COLOR), value(value), l(l), r(r), p(p) {}

    // a classe Map e Iterator são amigas da classe Node,
    // permitindo que elas acessem seus membros privados.
    // Isso é útil para operações de inserção, remoção e iteração,
    // onde Map e Iterator precisam acessar diretamente os dados do nó.
    friend class Iterator<K, V>;
    friend class Map<K, V>;
    
    ~Node() {}

};

#endif