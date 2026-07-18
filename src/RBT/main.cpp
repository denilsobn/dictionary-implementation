#include <iostream>
#include "diciionario-RBT.hpp"
#include <fstream>
#include <vector>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/brkiter.h>
#include <unicode/unistr.h>
#include <unicode/regex.h>
#include <ctime>
#include <filesystem>
#include <iomanip>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc != 2) {
        return 1;
    }
    string book = argv[1];

    filesystem::path exe_path = filesystem::read_symlink("/proc/self/exe");

    filesystem::path exe_d = exe_path.parent_path().parent_path();

    string base = exe_d.string();

    string directory = base + "/books/" + book;

    ifstream file(directory);

    if (!file.is_open()) {
        cerr << "falha a abrir o arquivo: " << directory << "\n";
        return 1;
    }

    UErrorCode status = U_ZERO_ERROR;
    
    auto pattern = icu::RegexPattern::compile(
        "\\p{L}+(?:[\\p{Pd}]\\p{L}+)*", 0, status
    );

    if (U_FAILURE(status)) {
        cerr << "Erro ao criar o BreakIterator: " << u_errorName(status) << "\n";
        return 1;
    }

    string line;

    Map<icu::UnicodeString, int> unicode_word;
    vector<icu::UnicodeString> word;

    while(getline(file, line)) {
        
        icu::UnicodeString uline = icu::UnicodeString::fromUTF8(line);
        
        auto matcher = pattern->matcher(uline, status);
        if (U_FAILURE(status)) break;

        while (matcher->find(status)) {
            icu::UnicodeString safe = matcher->group(status);
            word.push_back(safe);
        }
    }

    file.close();

    string archive = base + "/count_file/rbt/" + book;

    ofstream out(archive);

    if (!out.is_open()) {
        cerr << "falha ao criar o arquivo: " << archive << "\n";
        return 1;
    }

    clock_t time_in = clock();

    for (auto& e : word) {
        unicode_word[e]++;
    }

    time_in = clock() - time_in;

    for (auto i = unicode_word.begin(); i != unicode_word.end(); ++i) {
        string uft8;
        i->first.toUTF8String(uft8);
        out << uft8 << ":"  << i->second << "\n";
    }

    clock_t time_find = clock();
    for (int i = 0; i < 1000; i++) unicode_word.find("Atílio");
    time_find = clock() - time_find;

    clock_t time_delete = clock();
    for (int i = 0; i < 1000; i++) unicode_word.remove("Atílio");
    time_delete = clock() - time_delete;

    out << "Quantidade de rotações: " << unicode_word.getRotations() << "\n";
    out << "Quantidade de comparações: " << unicode_word.getComparations() << "\n";
    out << "Quantidade de tempo para inserir: " << setprecision(2) << fixed 
                    << 1000 * (float)time_in/CLOCKS_PER_SEC << " milisegundos\n";
    out << "Quantidade de tempo para encontrar um elemento que não existe: " << 1000 * (float)time_find/CLOCKS_PER_SEC << "ms\n";
    out << "Quantidade de tempo para deletar um elemento que não existe: " << 1000 * (float)time_delete/CLOCKS_PER_SEC << "ms\n";

    out.close();

    cout << "salvo em: " << archive << "\n";

    return 0;

}
