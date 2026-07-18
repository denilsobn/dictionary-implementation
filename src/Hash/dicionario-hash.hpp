#ifndef DIC_HASH_HPP
#define DIC_HASH_HPP

#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <functional>
#include <sstream>
#include <algorithm>

template<typename Key, typename Value>
class UnorderedMap {
private:
    std::vector<std::list<std::pair<Key, Value>>> table;
    size_t size;
    _Float64 maxLoadFactor;
    std::hash<Key> hashing;
    int comparations = 0, rehashes = 0;

    static size_t getNextPrime(size_t n) {
        if (n <= 1) return 2;
        if (n % 2 == 0) n++;
        while (true) {
            bool isPrime = true;
            for (size_t i = 3; i * i <= n; i += 2) {
                if (n % i == 0) {
                    isPrime = false;
                    break;
                }
            }
            if (isPrime) return n;
            n += 2;
        }
    }


    // Função de hash padrão usando std::hash
    size_t hashFunction(const Key& key) const {
        return hashing(key) % table.size();
    }

    float getLoadFactor() const {
        return static_cast<float>(size) / table.size();
    }

    void _clear() {
        for (auto& bucket : table) {
            bucket.clear();
        }
        size = 0;
    }

    // rehashing da tabela de hash, dobra o tamanho da tabela e redistribui os elementos
    // isso é necessário quando o fator de carga (load factor) ultrapassa o máximo permitido
    // para evitar colisões excessivas e manter a eficiência da tabela de hash
    void rehash() {
        size_t newSize = getNextPrime(table.size() * 2);
        std::vector<std::list<std::pair<Key, Value>>> newTable(newSize);

        for (auto& bucket : table) {
            for (auto& pair : bucket) {
                size_t newIndex = hashing(pair.first) % newSize;
                newTable[newIndex].push_back(pair);
            }
        }
        table = std::move(newTable);
    }

    // insere um novo par chave-valor na tabela de hash
    // se a chave já existir, não faz nada, optei por não sobrescrever o valor
    // porque o operador [] já faz isso
    void _insert(const Key& key, const Value& value) {
        if (getLoadFactor() > maxLoadFactor) {
            rehashes++;
            rehash();
        }
        size_t index = hashFunction(key);
        for (auto& pair : table[index]) {
            comparations++;
            if (pair.first == key) {
                return;
            }
        }
        table[index].push_back({key, value});
        size++;
    }

    // como eu optei por não usar iteradores, usei ponteiros para retornar o valor
    // se a chave não existir, retorna nullptr
    // se a chave existir, retorna um ponteiro para o valor associado à chave
    // isso é útil para evitar cópias desnecessárias de valores grandes
    // e para permitir que o usuário modifique o valor diretamente
    Value* _find(const Key& key) {
        size_t index = hashFunction(key);

        for (auto& pair : table[index]) {
            comparations++;
            if (pair.first == key) {
                Value* value = new Value(pair.second); 
                return value;
            }
        }

        return nullptr;
    }

    // remove um par chave-valor da tabela de hash
    // se a chave não existir, retorna nullptr
    // se a chave existir, remove o par e retorna um ponteiro para o valor associado
    Value* _erase(const Key& key) {
        size_t index = hashFunction(key);
        auto& bucket = table[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            comparations++;
            if (it->first == key) {
                Value* temp = new Value(it->second); // cria uma cópia do valor para retornar
                bucket.erase(it);
                size--;
                return temp;
            }
        }
        return nullptr;
    }

    // função auxiliar para mostrar o conteúdo da tabela de hash
    // os contadores não são incrementados aqui, pois é apenas uma função de exibição
    void _show() const {
        for (size_t i = 0; i < table.size(); ++i) {
            if (!table[i].empty()) {
                for (const auto& pair : table[i]) {
                    std::cout << "{" << pair.first << ":" << pair.second << "}" << std::endl;
                }
            }
        }
    }

    std::string _showString() const {
        std::vector<std::string> result;
        for (size_t i = 0; i < table.size(); ++i) {
            if (!table[i].empty()) {
                for (const auto& pair : table[i]) {
                    std::stringstream ss_k, ss_v;
                    ss_k << pair.first;
                    ss_v << pair.second;
                    result.push_back(ss_k.str() + ":" + ss_v.str() + "\n");
                }
            }
        }
        sort(result.begin(), result.end());
        std::string ans = "";
        for (auto& elements : result) {
            ans += elements;
        }
        return ans;
    }


public:

    UnorderedMap(size_t sizeT = 71, float maxLoadFactor = 0.75) {
        table.resize(getNextPrime(sizeT));
        this->maxLoadFactor = maxLoadFactor;
        size = 0;
    }


    size_t sizeH() const { return size; }
    bool isEmpty() const { return size == 0; }
    int getComparations() const { return comparations; }
    int getRehashes() const { return rehashes; }

    // operador de indexação para acessar o valor associado a uma chave
    // se a chave não existir, ela é inserida com um valor padrão Value()
    // isso permite que o usuário acesse o valor diretamente usando a sintaxe map[key]
    // e também permite que o usuário modifique o valor diretamente
    // se a chave já existir, retorna o valor associado à chave
    // se a chave não existir, insere um novo par chave-valor com o valor padrão Value()
    // e retorna o valor associado à chave
    // isso é útil para evitar a necessidade de verificar se a chave existe antes de acessar o valor
    // e também para evitar a necessidade de chamar uma função separada para inserir um novo
    // par chave-valor
    Value& operator[](const Key& key) {
        size_t index = hashFunction(key);
        for (auto& pair : table[index]) {
            comparations++;
            if (pair.first == key) {
                return pair.second;
            }
        }
        _insert(key, Value());
        index = hashFunction(key); // pode ocorrer o rehash e o hashFuncion mudar
        return table[index].back().second;
    }

    void insert(const Key& key, const Value& value) {
        _insert(key, value);
    }

    Value* find(const Key& key) {
        return _find(key);
    }

    Value* erase(const Key& key) {
        return _erase(key);
    }

    void clear() {
        _clear();
    }

    void show() const {
        _show();
    }

    std::string showString () const {
        return _showString();
    }

    ~UnorderedMap() {
        clear();
    }

};

#endif