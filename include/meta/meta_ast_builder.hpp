#pragma once

#include "meta/token.hpp"
#include "utils/stack.hpp"
#include "meta/meta_ast.h"
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
    void applyAction(int code, const meta::Token& token);
    void printAST(const std::shared_ptr<Node>& node, int indent = 0) const;
    void printAllASTs() const;
    std::unordered_map<std::string, std::shared_ptr<Node>> getASTMap() const { return astMap; }
    void determineRootRule();
    const std::string& getStartRule() const { return startRule; }

private:
    
    Logger& logger;
    Stack<std::shared_ptr<Node>> stack;
    std::unordered_map<std::string, std::shared_ptr<Node>> astMap;
    std::string startRule;
    void collectNonTerminals(const std::shared_ptr<Node>& node, std::unordered_set<std::string>& referencedRules) const;


};
