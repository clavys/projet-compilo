#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>

#include "compilo/lexer.hpp"
#include "compilo/parser.hpp"
#include "compilo/runtime.hpp"
#include "meta/token.hpp"
#include "meta/meta_lexer.hpp"
#include "meta/meta_parser.hpp"
#include "utils/logger.hpp"

using namespace compilo;

int main() {
    Logger logger(Logger::Level::Error); // Changer le niveau de log ici
    Logger métaLogger(Logger::Level::Error); // Changer le niveau de log ici
    Logger runtimeLogger(Logger::Level::Error); // Changer le niveau de log ici

    namespace fs = std::filesystem;

    // =========================
    // 1) Charger la grammaire
    // =========================

    fs::path grammarPath = "../grammars/gpl.gram";
    std::shared_ptr<MetaParser> metaParser;

    if (!fs::exists(grammarPath)) {
        métaLogger.error("Fichier .gram introuvable : " + grammarPath.string());
        return 1;
    }

    std::ifstream grammarFile(grammarPath);
    std::string grammarSource((std::istreambuf_iterator<char>(grammarFile)),
                               std::istreambuf_iterator<char>());

    // =========================
    // 2) Générer l'AST de la grammaire
    // =========================

    try {
        meta::MetaLexer metaLexer(grammarSource);
        auto metaTokens = metaLexer.tokenize();

        metaParser = std::make_shared<MetaParser>(metaTokens, métaLogger);

        metaParser->generateGrammarAST();
        
    } catch (const std::exception& e) {
        métaLogger.error("Erreur lecture grammaire : " + std::string(e.what()));
        return 1;
    }

    // =========================
    // 3) Charger le code source
    // =========================

    std::ifstream srcFile("../examples/tri_fusion.mylang");
    if (!srcFile) {
        logger.error("Impossible d'ouvrir le fichier source");
        return 1;
    }

    std::stringstream buffer;
    buffer << srcFile.rdbuf();
    std::string source = buffer.str();

    // =========================
    // 4) Générer le Pcode du code source
    // =========================

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    std::cout << "\n=== Tokens du programme ===\n";
    for (const auto& tok : tokens) {
        std::cout << tok << "\n";
    }

    std::string startRule = metaParser->getStartRule();
    auto astMap = metaParser->getASTMap();

    Parser parser(tokens, astMap, startRule, logger);
    bool parseSuccess = parser.generatePcode();

    if (!parseSuccess) {
        std::cerr << "\nParsing echoue \n";
        return 1;
    }

    std::cout << "\n=== Parsing reussi  ===\n";

    // =========================
    // 5) Exécuter le PCode généré
    // =========================
    Runtime rt(runtimeLogger);

    std::vector<Instruction> program = parser.getPcode();

    if (rt.execute(program)) {
        std::cout << "\nExecution terminee sans erreur \n";
        if( runtimeLogger.getLevel() == Logger::Level::Debug ){
        rt.dumpVariables();
        rt.dumpStack();
    }
    } else {
        std::cerr << "\nErreur pendant l'execution \n";
    }

    return 0;
}
