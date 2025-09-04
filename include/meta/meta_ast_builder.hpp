#pragma once

#include "meta/token.hpp"
#include "utils/stack.hpp"
#include "ast/ast.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "utils/logger.hpp"

class ASTBuilder {
public:
    ASTBuilder(Logger& logger) : logger(logger) {}
    void applyAction(int code, const meta::Token& token); // Applique une action en fonction du code ex : #100, #200, etc.
    void printAST(const std::shared_ptr<Node>& node, int indent = 0) const; // Pour le débogage
    void printAllASTs() const; // Pour le débogage
    std::unordered_map<std::string, std::shared_ptr<Node>> getASTMap() const { return astMap; } // Récupère la map des ASTs (règles de la grammaire)
    std::shared_ptr<Node> getAST(const std::string& ruleName) const; // Récupère l'AST d'une règle spécifique   
    void determineRootRule(); // Détermine la règle de départ
    const std::string& getStartRule() const { return startRule; } // Récupère la règle de départ

private:
    
    Logger& logger;
    Stack<std::shared_ptr<Node>> stack;
    std::unordered_map<std::string, std::shared_ptr<Node>> astMap;
    std::string startRule;
    void collectNonTerminals(const std::shared_ptr<Node>& node, std::unordered_set<std::string>& referencedRules) const; // Collecte les règles non terminales référencées dans un AST


};
