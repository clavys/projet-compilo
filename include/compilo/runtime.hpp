#pragma once
#include <iostream>
#include <stack>
#include <unordered_map>
#include <variant>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "utils/logger.hpp"
#include "compilo/pcodegen.hpp"
#include "compilo/instruction.hpp"
#include "compilo/opcode.hpp"

// Déclaration récursive
struct Value;
using List = std::shared_ptr<std::vector<Value>>;

struct Value : public std::variant<int, double, bool, std::string, List> {
    using variant::variant;
};

// Structure pour les frames de fonctions
struct Frame {
    std::unordered_map<std::string, Value> localVars;
    std::unordered_map<std::string, Value> params;
    std::vector<std::string> paramNames;
    size_t returnAddress;
    
    Frame(size_t retAddr) : returnAddress(retAddr) {}
};

class Runtime {
public:
    Runtime(Logger& logger);

    bool execute(const std::vector<Instruction>& instructions);
    void reset();

    // Debug
    void dumpStack() const;
    void dumpVariables() const;
    void dumpCallStack() const;

private:
    Logger& logger;
    std::stack<Value> stack;
    std::unordered_map<std::string, Value> variables;

    size_t pc;
    bool error;
    std::string lastError;

    // Gestion des fonctions
    std::stack<Frame> callStack;
    std::unordered_map<std::string, size_t> functions; // nom -> adresse

    // Pour accéder aux instructions dans executeFuncDecl
    const std::vector<Instruction>* instructions;

    // Utilitaires pile
    void setError(const std::string& message);
    void push(const Value& value);
    Value pop();
    Value peek() const;
    bool isEmpty() const;

    // Conversions
    std::string valueToString(const Value& value) const;
    bool valueToBool(const Value& value) const;
    double valueToNumber(const Value& value) const;

    // Instructions
    void executeLoad(const std::string& name);
    void executeStore(const std::string& name);

    void executeAdd();
    void executeSub();
    void executeMul();
    void executeDiv();
    void executeMod();
    void executeNeg();

    void executeAnd();
    void executeOr();
    void executeNot();

    void executeEq();
    void executeNeq();
    void executeLt();
    void executeGt();
    void executeLe();
    void executeGe();

    void executeMakeList(int size);
    void executeListGet();
    void executeListSet();
    void executeListLen();
    void executeMakeEmptyList();

    void executePrintString(Instruction instr);
    void executePrintBool(Instruction instr);
    void executePrintNumber(Instruction instr);

    void executeLoadLocal(const std::string& name);
    void executeStoreLocal(const std::string& name);
    void executeDeclareParam(const std::string& name);
    void executeFuncDecl(const std::string& name);
    void executePushFrame();
    void executePopFrame();
    void executeArgCount(int count);
    void executeCall(const std::string& funcName);
    void executeReturn();
    void executeReturnVoid();
    void executeFuncEnd();


};
