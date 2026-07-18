
#ifndef NODE_HPP
#define NODE_HPP

#include <utility>

template <typename K, typename V>
class AvlMap;
template <typename K, typename V>
class Iterator;

template <typename Key, typename Value>
class Node
{
public:
    Node(const std::pair<Key, Value> &v, short h = 1,
         Node *l = nullptr, Node *r = nullptr, Node *p = nullptr)
        : value(v), h(h), l(l), r(r), p(p) {}

    ~Node() = default;

private:
    std::pair<Key, Value> value;
    short h;
    Node *l;
    Node *r;
    Node *p;
    
    // A classe Map e Iterator são amigas da classe Node,
    // permitindo que elas acessem seus membros privados.
    // Isso é útil para operações de inserção, remoção e iteração,
    // onde Map e Iterator precisam acessar diretamente os dados do nó.
    friend class AvlMap<Key, Value>;
    friend class Iterator<Key, Value>;
};

#endif