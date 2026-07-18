#ifndef DIC_RB_HPP
#define DIC_RB_HPP

#include "node.hpp"
#include "iterator.hpp"

#define BLACK '1'
#define RED '2'

template <typename Key, typename Value>
class Map
{
private:
    Node<Key, Value> *root;
    static Node<Key, Value> *NIL;
    size_t _size;
    int rotations = 0, comparations = 0;

    Node<Key, Value> *find(const Key& k, Node<Key, Value> *node)
    {

        Node<Key, Value> *curr = node;

        while (curr != NIL)
        {
            comparations++;
            if (curr->value.first == k)
            {
                return curr;
            }
            else if (curr->value.first < k)
            {
                curr = curr->r;
            }
            else if (curr->value.first > k)
            {
                curr = curr->l;
            }
        }

        return curr;
    }

    void clearRec(Node<Key, Value> *node)
    {
        if (node != NIL)
        {
            clearRec(node->l);
            clearRec(node->r);
            delete node;
        }
    }

    // função que faz uma rotação à esquerda
    // ponteiro para pai é atualizado
    // e o nó filho direito se torna o novo pai
    // retorna o novo nó pai
    Node<Key, Value> *leftRotation(Node<Key, Value> *node)
    {

        Node<Key, Value> *aux = node->r;
        node->r = aux->l;
        if (aux->l != NIL)
            aux->l->p = node;
        aux->p = node->p;
        if (node->p == NIL)
        {
            root = aux;
        }
        else if (node == node->p->l)
        {
            node->p->l = aux;
        }
        else
        {
            node->p->r = aux;
        }
        aux->l = node;
        node->p = aux;
        return aux;
    }

    // função que faz uma rotação à direita
    // ponteiro para pai é atualizado
    // e o nó filho esquerdo se torna o novo pai
    // retorna o novo nó pai
    Node<Key, Value> *rightRotation(Node<Key, Value> *node)
    {

        Node<Key, Value> *aux = node->l;
        node->l = aux->r;
        if (aux->r != NIL)
            aux->r->p = node;
        aux->p = node->p;
        if (node->p == NIL)
        {
            root = aux;
        }
        else if (node == node->p->r)
        {
            node->p->r = aux;
        }
        else
        {
            node->p->l = aux;
        }
        aux->r = node;
        node->p = aux;
        return aux;
    }

    // função que corrige o nó após a inserção
    // separada em três partes:
    // 1. Se o nó pai é vermelho, verifica o tio
    //    e faz as correções necessárias.
    // 2. Se o nó pai é vermelho e o tio é preto,
    //    faz rotações para manter o balanceamento.
    // 3. Se o nó pai é vermelho e o tio é vermelho,
    //    muda as cores do pai e tio para preto
    //    e sobe o nó para o 
    //    fazendo as rotações necessárias.
    void fixNode(Node<Key, Value> *node)
    {
        while (node != root && node->p->COLOR == RED)
        {
            if (node->p == node->p->p->l)
            {
                Node<Key, Value> *uncle = node->p->p->r;
                if (uncle->COLOR == RED)
                {
                    node->p->COLOR = BLACK;
                    uncle->COLOR = BLACK;
                    node->p->p->COLOR = RED;
                    node = node->p->p;
                }
                else
                {
                    if (node == node->p->r)
                    {
                        node = node->p;
                        leftRotation(node);
                        rotations++;
                    }
                    node->p->COLOR = BLACK;
                    node->p->p->COLOR = RED;
                    rightRotation(node->p->p);
                    rotations++;
                }
            }
            else
            {
                Node<Key, Value> *uncle = node->p->p->l;
                if (uncle->COLOR == RED)
                {
                    node->p->COLOR = BLACK;
                    uncle->COLOR = BLACK;
                    node->p->p->COLOR = RED;
                    node = node->p->p;
                }
                else
                {
                    if (node == node->p->l)
                    {
                        node = node->p;
                        rightRotation(node);
                        rotations++;
                    }
                    node->p->COLOR = BLACK;
                    node->p->p->COLOR = RED;
                    leftRotation(node->p->p);
                    rotations++;
                }
            }
        }
        root->COLOR = BLACK;
    }

    void fixDeleteNode(Node<Key, Value> *x) {
        while(x != root && x->COLOR == BLACK) {
            if (x == x->p->l) {
                Node<Key, Value> *w = x->p->r;
                if (w->COLOR == RED) {
                    w->COLOR = BLACK;
                    x->p->COLOR = RED;
                    leftRotation(x->p);
                    w = x->p->r;
                }
                if (w->l->COLOR == BLACK && w->r->COLOR == BLACK) {
                    w->COLOR = BLACK;
                    x = x->p;
                }
                else {
                    if (w->r->COLOR == BLACK) {
                        w->l->COLOR = BLACK;
                        w->COLOR = RED;
                        rightRotation(w);
                        w = x->p->r;
                    }
                    w->COLOR = x->p->COLOR;
                    x->p->COLOR = BLACK;
                    w->r->COLOR = BLACK;
                    leftRotation(x->p);
                    x = root;
                }
            }
            else {
                Node<Key, Value> *w = x->p->l;
                if (w->COLOR == RED) {
                    w->COLOR = BLACK;
                    x->p->COLOR = RED;
                    rightRotation(x->p);
                    w = x->p->l;
                }
                if (w->r->COLOR == BLACK && w->l->COLOR == BLACK) {
                    w->COLOR = BLACK;
                    x = x->p;
                }
                else {
                    if (w->l->COLOR == BLACK) {
                        w->r->COLOR = BLACK;
                        w->COLOR = RED;
                        leftRotation(w);
                        w = x->p->l;
                    }
                    w->COLOR = x->p->COLOR;
                    x->p->COLOR = BLACK;
                    w->l->COLOR = BLACK;
                    rightRotation(x->p);
                    x = root;
                }   
            }
        }
        x->COLOR = BLACK;
    }

    // função que substitui o nó u pelo nó v na árvore
    // se u for a raiz, v se torna a nova raiz
    // se u for filho esquerdo de seu pai, v se torna o novo filho esquerdo
    // se u for filho direito de seu pai, v se torna o novo filho direito
    // o ponteiro do pai de v é atualizado para o pai de u
    void transplant(Node<Key, Value> *u, Node<Key, Value> *v)
    {
        if (u->p == NIL)
        {
            root = v;
        }
        else if (u == u->p->l)
        {
            u->p->l = v;
        }
        else
        {
            u->p->r = v;
        }
        v->p = u->p;
    }

    Node<Key, Value> *minimum(Node<Key, Value> *node)
    {
        while (node->l != NIL)
        {
            comparations++;
            node = node->l;
        }
        return node;
    }

    Node<Key, Value> *maximum(Node<Key, Value> *node)
    {
        while (node->r != NIL)
        {
            comparations++;
            node = node->r;
        }
        return node;
    }

    Node<Key, Value> *insert(const Key &k, const Value &v, Node<Key, Value> *node)
    {

        Node<Key, Value> *curr = node;
        Node<Key, Value> *parent = NIL;

        while (curr != NIL)
        {
            comparations++;
            parent = curr;
            if (curr->value.first < k)
            {
                curr = curr->r;
            }
            else if (curr->value.first > k)
            {
                curr = curr->l;
            }
            else
                return node;
        }

        Node<Key, Value> *newNode = new Node<Key, Value>(RED, {k, v}, NIL, NIL, parent);

        if (parent == NIL)
        {
            root = newNode;
        }
        else if (k < parent->value.first)
        {
            parent->l = newNode;
        }
        else
        {
            parent->r = newNode;
        }
        comparations++;

        fixNode(newNode);

        return newNode;
    }

    // função que remove um nó da árvore
    // dividida em quatro casos:
    // mostrados no livro "Algoritmos e Estruturas de Dados" de Cormen, Leiserson, Rivest e Stein
    // 1. Se o nó a ser removido é vermelho e não tem filhos, basta removê-lo.
    // 2. Se o nó a ser removido é vermelho e tem um filho,
    //    basta substituir o nó pelo seu filho.
    // 3. Se o nó a ser removido é vermelho e tem dois filhos,
    //    substituímos o nó pelo seu sucessor (o menor nó da subárvore direita)
    //    e removemos o sucessor.
    // 4. Se o nó a ser removido é preto e não tem filhos,
    //    basta removê-lo e seu pai se tornará preto.
    //    Se o pai também for preto, a árvore precisa ser corrigida(duplo preto).
    // 5. Se o nó a ser removido é preto e tem um filho,
    //    substituímos o nó pelo seu filho e corrigimos a árvore.
    // 6. Se o nó a ser removido é preto e tem dois filhos,
    //    substituímos o nó pelo seu sucessor (o menor nó da subárvore direita)
    //    e removemos o sucessor, corrigindo a árvore.
    // A função também lida com o caso em que o nó a ser removido é a raiz da árvore.
    // Se o nó a ser removido é a raiz, a árvore é corrigida para manter a propriedade de árvore vermelha-preta.
    void _remove(const Key &k)
    {
        Node<Key, Value> *z = find(k, root);
        if (z == NIL)
            return;

        Node<Key, Value> *y = z;
        Node<Key, Value> *x;
        char y_original_color = y->COLOR;

        if (z->l == NIL)
        {
            x = z->r;
            transplant(z, z->r);
        }
        else if (z->r == NIL)
        {
            x = z->l;
            transplant(z, z->l);
        }
        else
        {
            y = minimum(z->r);
            y_original_color = y->COLOR;
            x = y->r;
            if (y->p == z)
            {
                x->p = y;
            }
            else
            {
                transplant(y, y->r);
                y->r = z->r;
                y->r->p = y;
            }
            transplant(z, y);
            y->l = z->l;
            y->l->p = y;
            y->COLOR = z->COLOR;
        }
        delete z;
        if (y_original_color == BLACK)
        {
            fixDeleteNode(x);
        }
    }

    void bshow(Node<Key, Value> *node, std::string heranca) {
        if(node != NIL && (node->l != NIL || node->r != NIL))
            bshow(node->r , heranca + "r");
        for(int i = 0; i < (int) heranca.size() - 1; i++)
            std::cout << (heranca[i] != heranca[i + 1] ? "│   " : "    ");
        if(heranca != "")
            std::cout << (heranca.back() == 'r' ? "┌───" : "└───");
        if(node == NIL){
            std::cout << "#" << std::endl;
            return;
        }
        std::cout << "(" << node->value.first << "," << node->value.second << "," << node->COLOR << ")" << std::endl;
        if(node != NIL && (node->l != NIL || node->r != NIL))
            bshow(node->l, heranca + "l");
    }

public:
    friend class Iterator<Key, Value>;
    using iterator = Iterator<Key, Value>;

    iterator begin() { return iterator(root); }
    iterator end() { return NIL; }

    Map()
    {
        root = NIL;
    }

    // operador de indexação para acessar o valor associado a uma chave
    // se a chave não existir, ela é inserida com um valor padrão Value()
    // isso permite que o usuário acesse o valor diretamente usando a sintaxe map[key]
    // e também permite que o usuário modifique o valor diretamente
    Value &operator[](const Key& k)
    {
        Node<Key, Value> *node = find(k, root);

        if (node == NIL)
        {
            node = insert(k, Value(), root);
        }

        return node->value.second;
    }

    const iterator *find(const Key &k)
    {
        Node<Key, Value> *node = find(k, root);
        return new iterator(node);
    }

    void insert(const Key &k, const Value &v)
    {
        insert(k, v, root);
        _size++;
    }

    void remove(const Key &k)
    {
        _remove(k);
    }

    void clear()
    {
        clearRec(root);
        root = NIL;
        _size = 0;
    }

    void bshow()
    {
        if (root != NIL)
            bshow(root, "");
        else
            std::cout << "Empty tree" << std::endl;
    }

    size_t size() const { return _size; }
    bool isEmpty() const { return _size == 0; }
    int getRotations() const { return rotations; }
    int getComparations() const { return comparations; }

    ~Map()
    {
        clear();
    }
};

// Como a atributo NIL é estático, precisamos inicializá-lo fora da classe
template <typename K, typename V>
Node<K, V> *Map<K, V>::NIL = new Node<K, V>(BLACK);

#endif