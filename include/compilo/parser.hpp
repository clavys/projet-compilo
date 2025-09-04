#pragma once
#include "meta/meta_ast_builder.hpp"
#include "compilo/token.hpp"
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "utils/stack.hpp"
#include "ast/ast.h"
#include "utils/logger.hpp"
#include "compilo/pcodegen.hpp"

class Parser {
public:
    Parser(std::vector<compilo::Token> tokens,std::unordered_map<std::string, std::shared_ptr<Node>> astMap,std::string rootRule , Logger& logger)
        : tokenList(std::move(tokens)), builder(logger), logger(logger), rootRule(rootRule), grammarRules(astMap) {}
    void setLogLevel(Logger::Level level) { logger.setLevel(level); }
    bool analyse(const std::shared_ptr<Node>& node, std::vector<compilo::Token>* tokens, int* index);
    bool generatePcode();
    std::vector<Instruction> getPcode() const { return builder.getInstructions(); }
    bool parseImportedFile(const std::string& filename, const std::string& prefix);

    
    

    

private:
    std::string rootRule;
    std::unordered_map<std::string, std::shared_ptr<Node>> grammarRules;
    Stack<std::shared_ptr<Node>> stack;
    Logger& logger;
    std::vector<compilo::Token> tokenList;
    int stepCounter = 0;
    PCodegen builder;
    std::unordered_set<std::string> importedModules;
    

    
};