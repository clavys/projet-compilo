#pragma once
#include "meta/meta_ast_builder.hpp"
#include "meta/token.hpp"
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "utils/stack.hpp"
#include "ast/ast.h"
#include "utils/logger.hpp"

class MetaParser {
public:
    MetaParser(std::vector<meta::Token> tokens, Logger& logger)
        : tokenList(std::move(tokens)), builder(logger), logger(logger) {}
    void setLogLevel(Logger::Level level) { logger.setLevel(level); }
    bool analyse(const std::shared_ptr<Node>& node, std::vector<meta::Token>* tokens, int* index); // Analyse récursive descendante
    bool generateGrammarAST(); // Génère l'AST de la grammaire
    void printAllRules() const { builder.printAllASTs(); } // Pour le débogage
    std::unordered_map<std::string, std::shared_ptr<Node>> getASTMap() const { return builder.getASTMap(); }
    std::string getStartRule() const { return builder.getStartRule(); }

private:
    Logger& logger;
    std::vector<meta::Token> tokenList;
    int stepCounter = 0;
    ASTBuilder builder;

    
};