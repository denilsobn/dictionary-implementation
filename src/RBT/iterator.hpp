#ifndef ITERATOR_HPP
#define ITERATOR_HPP

template<typename Key, typename Value> class Node;
template<typename Key, typename Value> class Map;

template<typename Key, typename Value>
class Iterator {
private:
    Node<Key, Value>* current;

    // função que funciona da mesma maneira que da classe AVL
    static Node<Key, Value>* minimum(Node<Key, Value>* node) {
        while (node != Map<Key, Value>::NIL && node->l != Map<Key, Value>::NIL) node = node->l;
        return node;
    }

    static Node<Key, Value>* successor(Node<Key, Value>* node) {
        if (node->r != Map<Key, Value>::NIL)
            return minimum(node->r);
        Node<Key, Value>* parent = node->p;
        while (parent != Map<Key, Value>::NIL && node == parent->r) {
            node = parent;
            parent = parent->p;
        }
        return parent;
    }

public:
    Iterator(Node<Key, Value>* root)
        : current(root ? minimum(root) : Map<Key, Value>::NIL) {}

    Iterator() : current(Map<Key, Value>::NIL) {}

    const std::pair<Key, Value>& operator*() const {
        return current->value;
    }

    const std::pair<Key, Value>* operator->() const {
        return &current->value;
    }

    Iterator& operator++() {
        if (current != Map<Key, Value>::NIL) current = successor(current);
        return *this;
    }

    bool operator==(const Iterator& i) const {
        return current == i.current;
    }
    bool operator!=(const Iterator& i) const {
        return current != i.current;
    }
};

#endif