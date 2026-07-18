#ifndef HASH_2_HPP
#define HASH_2_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <functional>
#include <sstream>
#include <algorithm>

#define BUSY '0'
#define EMPTY '1'
#define DELETED '2'

template<typename Key, typename Value>
class UnorderedOpenMap {
private:
    size_t size;
    size_t m;
    std::vector<std::pair<char, std::pair<Key, Value>>> table;
    std::hash<Key> hashing;
    int comparations = 0, rehashes = 0;

    static size_t getPreviusPrime(size_t n) {
        if (n % 2 == 0) n--;
        if (n <= 2) return 2;

        for (size_t i = n; i > 2; i -= 2) {
            bool isPrime = true;
            for (size_t j = 3; j * j <= i; j += 2) {
                if (i % j == 0) {
                    isPrime = false;
                    break;
                }
            }
            if (isPrime) return i;
        }
        return 2;
    }

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

    size_t hash1(const Key& key) const {
        return hashing(key) % table.size();
    }

    // A segunda função de hash é usada para resolver colisões
    // usando o método de endereçamento duplo.
    // Ela é baseada na primeira função de hash, mas adiciona 1
    // para garantir que o índice seja diferente do primeiro.
    size_t hash2(const Key& key) const {
        return 1 + (hashing(key) % m);
    }

    float loadFactor() {
        return static_cast<float>(size) / table.size();
    }

    void _clear() {
        table.clear();
        size = 0;
    }

    // recoloca todos os elementos da tabela de hash
    // em uma nova tabela de tamanho dobrado, usando o próximo número primo
    void rehash() {
        size_t newSize = getNextPrime(table.size() * 2);
        size_t m_new = getPreviusPrime(newSize);
        std::vector<std::pair<char, std::pair<Key, Value>>> newTable(
            newSize, {EMPTY, {Key(), Value()}}
        );

        for (const auto& cell : table) {
            if (cell.first == BUSY) {
                size_t i1 = hashing(cell.second.first) % newTable.size();
                size_t i2 = 1 + (hashing(cell.second.first) % m_new);

                for (size_t i = 0; ; ++i) {
                    size_t idx = (i1 + i * i2) % newTable.size();
                    if (newTable[idx].first == EMPTY) {
                        newTable[idx] = cell;
                        break;
                    }
                }
            }
        }
        table = std::move(newTable);
        m = m_new;
    }

    // // retorna ponteiro direto para o valor dentro da tabela
    //     função antiga
    //     Value* _find(const Key& key) {
    //         size_t i1 = hash1(key);
    //         size_t i2 = hash2(key);

    //         for (size_t i = 0; i < table.size(); ++i) {
    //             size_t idx = (i1 + i * i2) % table.size();
    //             if (table[idx].first == BUSY) {
    //                 comparations++;
    //                 if (table[idx].second.first == key) {
    //                     return &table[idx].second.second;
    //                 }
    //             }
    //         }
    //         return nullptr;
    //     }

    Value* _find(const Key& key) {
        size_t i1 = hash1(key);
        size_t i2 = hash2(key);

        for (size_t i = 0; i < table.size(); ++i) {
            size_t idx = (i1 + i * i2) % table.size();

            if (table[idx].first == EMPTY) {
                comparations++;
                return nullptr;
            }

            if (table[idx].first == BUSY) {
                comparations++;
                if (table[idx].second.first == key) {
                    return &table[idx].second.second;
                }
            }

        }
        return nullptr;
    }

    // insere um novo par chave-valor na tabela de hash
    // funcao antiga de inserção
        // void _insert(const Key& key, const Value& value) {
        //     if (loadFactor() >= 0.75) {
        //         rehash();
        //         rehashes++;
        //     }

        //     size_t i1 = hash1(key);
        //     size_t i2 = hash2(key);

        //     for (size_t i = 0; i < table.size(); ++i) {
        //         size_t idx = (i1 + i * i2) % table.size();
        //         if (table[idx].first == EMPTY) {
        //             table[idx].second = {key, value};
        //             table[idx].first = BUSY;
        //             ++size;
        //             return;
        //         }
        //     }
        // }

    int _insert(const Key& key, const Value& value) {
        if (loadFactor() >= 0.80) {
            rehash();
            rehashes++;
        }

        size_t i1 = hash1(key);
        size_t i2 = hash2(key);
        int f_deleted = -1;

        for (size_t i = 0; i < table.size(); ++i) {
            size_t idx = (i1 + i * i2) % table.size();

            if (table[idx].first == BUSY && table[idx].second.first == key) {
                comparations++;
                return idx;
            }

            if (table[idx].first == DELETED && f_deleted == -1) {
                comparations++;
                f_deleted = idx;
            }

            // insere no primeiro slot empty ou que foi deletado
            if (table[idx].first == EMPTY) {
                int pos = (f_deleted == -1 ? idx : f_deleted);
                table[pos].second = {key, value};
                table[pos].first = BUSY;
                ++size;
                return pos;
            }
        }
        // nunca vai chegar nessa
        return 0;
    }

    // antiga função de deleção
        // Value* _erase(const Key& key) {
        //     size_t i1 = hash1(key);
        //     size_t i2 = hash2(key);

        //     for (size_t i = 0; i < table.size(); ++i) {
        //         size_t idx = (i1 + i * i2) % table.size();
        //         if (table[idx].first == BUSY) {
        //             comparations++;
        //             if (table[idx].second.first == key) {
        //                 Value* old = new Value(table[idx].second.second);
        //                 table[idx].first = EMPTY;
        //                 --size;
        //                 return old;
        //             }
        //         }
        //     }
        //     return nullptr;
        // }

    Value* _erase(const Key& key) {
        size_t i1 = hash1(key);
        size_t i2 = hash2(key);

        for (size_t i = 0; i < table.size(); ++i) {
            size_t idx = (i1 + i * i2) % table.size();

            if (table[idx].first == EMPTY) {
                comparations++;
                return nullptr;
            }

            if (table[idx].first == BUSY) {
                comparations++;
                if (table[idx].second.first == key) {
                    Value* old = new Value(table[idx].second.second);
                    table[idx].first = DELETED;
                    --size;
                    return old;
                }
            }
        }
        return nullptr;
    }
    

    // função para mostrar o conteúdo da tabela de hash
    void _show() const {
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].first == BUSY) {
                std::cout << "Key: " << table[i].second.first
                          << ", Value: " << table[i].second.second << std::endl;
            }
        }
    }

    std::string _showString() const {
        std::vector<std::string> result;
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].first == BUSY) {
                std::stringstream ss_k, ss_v;
                ss_k << table[i].second.first;
                ss_v << table[i].second.second;
                result.push_back(ss_k.str() + ":" + ss_v.str() + "\n");
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
    UnorderedOpenMap<Key, Value>(size_t sizeT = 71) {
        m = getPreviusPrime(sizeT);
        table.resize(getNextPrime(sizeT), {EMPTY, {Key(), Value()}});
        size = 0;
    }

    size_t sizeH() const { return size; }
    bool isEmpty() const { return size == 0; }
    int getComparations() const { return comparations; }
    int getRehashes() const { return rehashes; }

    // operador de indexação para acessar o valor associado a uma chave
    Value& operator[](const Key& key) {

        size_t i1 = hash1(key);
        size_t i2 = hash2(key);

        int i_insert = 0;

        for (size_t i = 0; i < table.size(); ++i) {
            size_t idx = (i1 + i * i2) % table.size();

            if (table[idx].first == EMPTY) {
                i_insert = idx;
                break;
            }

            if (table[idx].first == BUSY) {
                comparations++;
                if (table[idx].second.first == key) {
                    return table[idx].second.second;
                }
            }

        }

        _insert(key, Value());
        return table[i_insert].second.second;

    }   

    // busca e retorna ponteiro para o valor, ou nullptr
    Value* find(const Key& key) {
        return _find(key);
    }

    // insere se não existir
    void insert(const Key& key, const Value& value) {
        if (!_find(key)) _insert(key, value);
    }

    // remove e retorna antigo valor
    Value* erase(const Key& key) {
        return _erase(key);
    }

    void clear() {
        _clear();
    }

    // exibe pares chave-valor
    void show() { _show(); }

    std::string showString() { return _showString(); }

    ~UnorderedOpenMap() {
        clear();
    }
};

#endif
