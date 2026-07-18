// Map.hpp
#ifndef DIC_AVL_HPP
#define DIC_AVL_HPP

#include <utility>
#include <algorithm>
#include "node.hpp"
#include "iterator.hpp"

template <typename Key, typename Value>
class AvlMap
{
private:
    Node<Key, Value> *root = nullptr;
    size_t _size;
    mutable int rotations, comparations;

    // esses métodos estão como funções auxiliares para o AVL
    // e não são parte do objeto público da classe Map.

    static size_t height(Node<Key, Value> *n)
    {
        return n ? n->h : 0;
    }

    static int balance(Node<Key, Value> *n)
    {
        return static_cast<int>(height(n->l)) - static_cast<int>(height(n->r));
    }

    // Rotations
    // Essas funções são usadas para manter o balanceamento da árvore AVL.

    static Node<Key, Value> *leftRotation(Node<Key, Value> *x)
    {
        Node<Key, Value> *y = x->r;
        x->r = y->l;
        if (y->l)
            y->l->p = x;
        y->l = x;
        y->p = x->p;
        x->p = y;
        x->h = 1 + std::max(height(x->l), height(x->r));
        y->h = 1 + std::max(height(y->l), height(y->r));
        return y;
    }

    static Node<Key, Value> *rightRotation(Node<Key, Value> *y)
    {
        Node<Key, Value> *x = y->l;
        y->l = x->r;
        if (x->r)
            x->r->p = y;
        x->r = y;
        x->p = y->p;
        y->p = x;
        y->h = 1 + std::max(height(y->l), height(y->r));
        x->h = 1 + std::max(height(x->l), height(x->r));
        return x;
    }

    // Função auxiliar para corrigir o nó após inserção ou remoção
    // Ela ajusta a altura do nó e realiza rotações se necessário.
    // Ela é chamada após cada inserção ou remoção para garantir que a árvore permaneça
    // balanceada.

    Node<Key, Value> *fixNode(Node<Key, Value> *n)
    {
        n->h = 1 + std::max(height(n->l), height(n->r));
        int b = balance(n);
        if (b > 1)
        {
            if (balance(n->l) < 0) {
                n->l = leftRotation(n->l);
                rotations++;
            }
            n = rightRotation(n);
            rotations++;
        }
        else if (b < -1)
        {
            if (balance(n->r) > 0) {
                n->r = rightRotation(n->r);
                rotations++;
            }
            n = leftRotation(n);
            rotations++;
        }
        return n;
    }

    void clearRec(Node<Key, Value> *n)
    {
        if (n)
        {
            clearRec(n->l);
            clearRec(n->r);
            delete n;
        }
    }

    Node<Key, Value> *insertRec(const Key &k, const Value &v,
                                Node<Key, Value> *n, Node<Key, Value> *parent)
    {
        if (!n)
        {
            return new Node<Key, Value>({k, v}, 1, nullptr, nullptr, parent);
        }
        if (k < n->value.first) {
            comparations++;
            n->l = insertRec(k, v, n->l, n);
        }
        else if (k > n->value.first) {
            comparations++;
            n->r = insertRec(k, v, n->r, n);
        }
        return fixNode(n);
    }

    // Função auxiliar para remover um nó da árvore AVL
    // Ela busca o nó com a chave k e o remove, ajustando a árvore para
    // manter o balanceamento. Se o nó não for encontrado nada acontece.

    Node<Key, Value> *removeRec(const Key &k, Node<Key, Value> *n)
    {
        if (!n)
            return nullptr;
        if (k < n->value.first)
        {
            n->l = removeRec(k, n->l);
            if (n->l)
                n->l->p = n;
        }
        else if (k > n->value.first)
        {
            n->r = removeRec(k, n->r);
            if (n->r)
                n->r->p = n;
        }
        else
        {
            if (!n->l || !n->r)
            {
                Node<Key, Value> *t = n->l ? n->l : n->r;
                if (t)
                    t->p = n->p;
                delete n;
                return t;
            }
            else
            {
                Node<Key, Value> *succ = n->r;
                while (succ->l)
                    succ = succ->l;
                n->value.first = succ->value.first;
                n->value.second = succ->value.second;
                n->r = removeRec(succ->value.first, n->r);
                if (n->r)
                    n->r->p = n;
            }
            _size--;
        }
        return fixNode(n);
    }

    Node<Key, Value> *search(Node<Key, Value> *n, const Key &k) const
    {
        while (n && n->value.first != k) {
            comparations++;
            n = (k < n->value.first) ? n->l : n->r;
        }
        return n;
    }

    void bshow(Node<Key, Value> *node, std::string heranca) {
        if(node != nullptr && (node->l != nullptr || node->r != nullptr))
            bshow(node->r , heranca + "r");
        for(int i = 0; i < (int) heranca.size() - 1; i++)
            std::cout << (heranca[i] != heranca[i + 1] ? "│   " : "    ");
        if(heranca != "")
            std::cout << (heranca.back() == 'r' ? "┌───" : "└───");
        if(node == nullptr){
            std::cout << "#" << std::endl;
            return;
        }
        std::cout << "(" << node->value.first << "," << node->value.second << ")" << std::endl;
        if(node != nullptr && (node->l != nullptr || node->r != nullptr))
            bshow(node->l, heranca + "l");
    }

public:
    AvlMap<Key, Value>() : root(nullptr), _size(0), rotations(0), comparations(0) {}

    using iterator = Iterator<Key, Value>;

    // iteradores para percorrer o mapa
    // eles são usados para iterar sobre os pares chave-valor na árvore AVL.
    // O iterador começa no nó mínimo da árvore e avança para o próximo nó sucess

    iterator begin() { return iterator(root); }
    iterator end() { return iterator(); }

    const iterator *find(const Key &k) const
    {
        Node<Key, Value> *n = search(root, k);
        return new iterator(n);
    }

    // operador de indexação para acessar o valor associado a uma chave
    // se a chave não existir, ela é inserida com um valor padrão Value()

    Value &operator[](const Key &k)
    {
        Node<Key, Value> *n = search(root, k);
        if (!n)
        {
            root = insertRec(k, Value(), root, nullptr);
            n = search(root, k);
        }
        return n->value.second;
    }

    void insert(const Key &k, const Value &v)
    {
        root = insertRec(k, v, root, nullptr);
        _size++;
    }

    void remove(const Key &k)
    {
        root = removeRec(k, root);
    }

    void clear()
    {
        clearRec(root);
        root = nullptr;
        _size = 0;
    }

    void bshow()
    {
        if (root)
            bshow(root, "");
        else
            std::cout << "Árvore vazia." << std::endl;
    }

    size_t size() const { return _size; }
    bool isEmpty() const { return _size == 0; }
    int getRotations() const { return rotations; }
    int getComparations() const { return comparations; }

    ~AvlMap()
    {
        clear();
    }
};

#endif
