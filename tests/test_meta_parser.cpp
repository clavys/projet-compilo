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

    fs::path filePath = "../grammars/g0.gram";  

    if (fs::exists(filePath) && filePath.extension() == ".gram") {
        std::cout << "\n=== Test fichier : " << filePath.filename() << " ===\n";

        std::ifstream input(filePath);
        if (!input) {
            logger.error("Impossible d'ouvrir le fichier : " + filePath.string());
            return 1;
        }

        std::string source((std::istreambuf_iterator<char>(input)),
                            std::istreambuf_iterator<char>());

        try {
            MetaLexer lexer(source);
            auto tokens = lexer.tokenize();
            // Affichage de tous les tokens
            std::cout << "\n=== Tokens générés ===\n";
            for (const auto& t : tokens) {
                std::cout << t << "\n";
            }
            MetaParser parser(tokens, logger); 
            parser.generateGrammarAST();
            parser.printAllRules();  // Affiche toutes les règles pour vérification
        } catch (const std::exception& e) {
            logger.error("Erreur dans " + filePath.filename().string() + " : " + e.what());
        }

    } else {
        logger.error("Fichier .gram introuvable : " + filePath.string());
        return 1;
    }


    return 0;
}
