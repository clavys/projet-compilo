#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>

#include "compilo/lexer.hpp"
#include "compilo/parser.hpp"
#include "meta/token.hpp"
#include "meta/meta_lexer.hpp"
#include "meta/meta_parser.hpp"
#include "utils/logger.hpp"

using namespace compilo;

int main() {
    Logger logger;  // Logger pour le parser du méta-compilateur
    Logger logger2; // Logger pour le parser du compilateur
    
    logger.setLevel(Logger::Level::Error); 
    logger2.setLevel(Logger::Level::Debug); // mets Debug pour voir les étapes

    namespace fs = std::filesystem;

    // =========================
    // 1) Charger la grammaire GPL
    // =========================
    fs::path filePath = "../grammars/gpl.gram";  
    std::shared_ptr<MetaParser> metaParser;

    if (fs::exists(filePath) && filePath.extension() == ".gram") {
        std::cout << "\n=== Lecture grammaire : " << filePath.filename() << " ===\n";

        std::ifstream gplInput(filePath);
        if (!gplInput) {
            logger.error("Impossible d'ouvrir le fichier : " + filePath.string());
            return 1;
        }

        std::string gplSource((std::istreambuf_iterator<char>(gplInput)),
                               std::istreambuf_iterator<char>());

        try {
            meta::MetaLexer metaLexer(gplSource);
            auto metaTokens = metaLexer.tokenize();

            metaParser = std::make_shared<MetaParser>(metaTokens, logger);
            metaParser->generateGrammarAST();
            //metaParser->printAllRules();    // Pour debug : afficher toutes les règles

        } catch (const std::exception& e) {
            logger.error("Erreur dans " + filePath.filename().string() + " : " + e.what());
            return 1;
        }
    } else {
        logger.error("Fichier .gram introuvable : " + filePath.string());
        return 1;
    }

    // =========================
    // 2) Charger le fichier source utilisateur
    // =========================
    std::ifstream srcInput("../examples/my_example.mylang");
    if (!srcInput) {
        std::cerr << "Impossible d'ouvrir le fichier ../examples/my_example.mylang" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << srcInput.rdbuf();
    std::string source = buffer.str();

    // Lexer du compilateur
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    std::cout << "\n=== Tokens du programme ===\n";
    for (const auto& tok : tokens) {
        std::cout << tok << "\n";
    }

    // =========================
    // 3) Récupérer infos grammaire
    // =========================
    std::string startRule = metaParser->getStartRule();
    std::unordered_map<std::string, std::shared_ptr<Node>> astMap = metaParser->getASTMap();

    std::cout << "\nRègle de départ : " << startRule << "\n";

    // =========================
    // 4) Parser le programme utilisateur
    // =========================
   
     
    Parser parser(tokens, astMap, startRule, logger2);

    bool success = parser.generatePcode();
    if (success) {
        std::cout << "\n=== Parsing réussi ===\n";
        
    } else {
        std::cerr << "\n!!! Parsing échoué !!!\n";
    }
  
    return 0;
}

