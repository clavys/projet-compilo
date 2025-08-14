#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include "meta/token.hpp"
#include "meta/meta_lexer.hpp"
#include "meta/meta_parser.hpp"
using namespace meta;

int main() {
    Logger logger;                  // Crée un logger global pour le test
    logger.setLevel(Logger::Level::Error); // Active le debug si tu veux

    namespace fs = std::filesystem;
    for (const auto& file : fs::directory_iterator("../grammars")) {
        if (file.path().extension() == ".gram") {
            std::cout << "\n=== Test fichier : " << file.path().filename() << " ===\n";
            std::ifstream input(file.path());
            std::string source((std::istreambuf_iterator<char>(input)),
                               std::istreambuf_iterator<char>());

            try {
                MetaLexer lexer(source);
                auto tokens = lexer.tokenize();
                MetaParser parser(tokens, logger); // Passe le logger au parser

                parser.generateGrammarAST();
            } catch (const std::exception& e) {
                logger.error("Erreur dans " + file.path().filename().string() + " : " + e.what());
            }
        }
    }

    return 0;
}
