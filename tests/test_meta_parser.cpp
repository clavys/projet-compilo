#include <iostream>
#include <fstream>
#include <vector>
#include <string>
// Update the path below if "token.hpp" is located elsewhere
#include "meta/token.hpp"
#include "meta/meta_lexer.hpp"
using namespace meta;

int main() {
    std::ifstream input("../grammars/base_lang.gram");
    if (!input.is_open()) {
        std::cerr << "Erreur : impossible d'ouvrir base_lang.gram\n";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(input)),
                       std::istreambuf_iterator<char>());

    MetaLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "Tokens extraits :\n";
    for (const auto& tok : tokens) {
        std::cout << tok << '\n';
    }

    return 0;
}
