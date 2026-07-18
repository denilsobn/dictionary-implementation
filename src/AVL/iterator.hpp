// Iterator.hpp
#ifndef ITERATOR_HPP
#define ITERATOR_HPP

template <typename Key, typename Value>
class Node;

template <typename Key, typename Value>
class Iterator
{
private:
    Node<Key, Value> *current;

    static Node<Key, Value> *minimum(Node<Key, Value> *node)
    {
        while (node && node->l) 
            node = node->l;
        return node;
    }

    // Função auxiliar para encontrar o sucessor de um nó na árvore
    // Ela retorna o nó com o menor valor maior que o nó atual.
    // Se o nó não tiver um filho direito, ela sobe na árvore até encontrar
    // um nó que seja o filho esquerdo de seu pai, retornando esse pai.
    // Se não houver sucessor, retorna nullptr.
    static Node<Key, Value> *successor(Node<Key, Value> *node)
    {
        if (node->r)
            return minimum(node->r);
        auto parent = node->p;
        while (parent && node == parent->r)
        {
            node = parent;
            parent = parent->p;
        }
        return parent;
    }

public:
    Iterator(Node<Key, Value> *root)
        : current(root ? minimum(root) : nullptr) {}

    Iterator() : current(nullptr) {}

    // operador de desreferência retorna o par chave-valor atual
    // ele permite acessar o valor associado à chave atual do iterador.
    // O operador * retorna uma referência ao par chave-valor atual,
    // enquanto o operador -> retorna um ponteiro para esse par.
    // Isso permite acessar os membros do par diretamente, como se fosse um objeto.
    // Por exemplo, se o iterador aponta para um nó com chave "key" e valor "value",
    // você pode usar *iterador para obter o par (key, value) e iterador->first para obter "key" e iterador->second para obter "value
    const std::pair<Key, Value> &operator*() const
    {
        return current->value;
    }

    const std::pair<Key, Value> *operator->() const
    {
        return &current->value;
    }

    Iterator &operator++()
    {
        if (current)
            current = successor(current);
        return *this;
    }

    bool operator==(const Iterator &i) const
    {
        return current == i.current;
    }
    bool operator!=(const Iterator &i) const
    {
        return current != i.current;
    }
};

#endif
