#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <iostream>
#include "utils/logger.hpp" 
#include <optional>
#include "compilo/opcode.hpp"
#include "compilo/instruction.hpp"
#include <unordered_set>


class PCodegen {

    struct FunctionInfo {
    std::string name;
    int paramCount = 0;
    std::vector<std::string> paramNames;
    std::string returnType = "unknown";
    bool typeInferred = false;
    };

    struct IfContext {
    size_t jumpIfFalseIndex;   // l’instruction JUMP_IF_FALSE générée après la condition
    size_t jumpIfFalseTarget;  // où doit aller JUMP_IF_FALSE (début du else OU fin du if)
    std::optional<size_t> jumpOverElseIndex;// l’instruction JUMP générée à la fin du bloc IF (si un else existe)
    size_t jumpOverElseTarget;// où doit aller ce JUMP (après le bloc else)
    };




public:
    PCodegen(Logger& logger) : logger(logger) {}

    
    std::vector<Instruction> getInstructions() const { return instructions; }
    

    // === Instructions ===
    void emit(OpCode opcode, const std::string& operand = "");
    void dump() const;

    // === Gestion des variables ===
    void declareVariable(const std::string& name, bool isLocal = false);
    void storeVariable(const std::string& name);
    void loadVariable(const std::string& name);

    // === Gestion des fonctions ===
    void declareFunction(const std::string& name, int paramCount);
    void callFunction(const std::string& name);

    // === Types ===
    void pushType(const std::string& type);
    std::string popType();
    std::string topType() const;

    // === Gestion des noms temporaires pour VarDecl / Assignment ===
    void pushVarName(const std::string& name);
    std::string popVarName(); 

    // === Lien avec parser ===
    void applyAction(int actionId, const std::string& value);

    // === Gestion des labels pour les sauts ===
    std::string newLabel(const std::string& prefix);
    void pushLabel(const std::string& lbl);
    std::string popLabel();


private:
    std::vector<Instruction> instructions;

    // Tables des symboles
    std::unordered_map<std::string, FunctionInfo> functionTable;
    std::unordered_map<std::string, std::string> symbolTable;

    // Gestion des portées
    std::unordered_set<std::string> localVariables;
    std::string currentFunction = "";

    // Piles de gestion
    std::stack<int> listSizeStack; // pour compter les éléments lors de la création de listes
    std::vector<int> rulesStack;    // pour gérer les prioritées dans les actions
    std::stack<std::string> typeStack;
    std::stack<std::string> varNameStack;// pile pour sauvegarder les noms de variables lors du parsing
    std::stack<std::string> callStack;

    // Gestion des structures de contrôle
    std::vector<size_t> patchStack;         // pour gérer les patchs de saut conditionnel
    std::vector<IfContext> ifStack;

    Logger& logger;
    bool hasError = false;
};