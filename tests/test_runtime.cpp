#include "compilo/runtime.hpp"
#include "utils/logger.hpp"



int main() {
    Logger logger(Logger::Level::Debug); // Changer le niveau de log ici
    Runtime rt(logger);

std::vector<Instruction> program = {
    {OpCode::PUSH, "5"},
    {OpCode::STORE, "i"},
    {OpCode::PUSH, "0"},
    {OpCode::STORE, "sum"},
    {OpCode::LOAD, "i"},
    {OpCode::PUSH, "0"},
    {OpCode::GT, ""},
    {OpCode::JUMP_IF_FALSE, "17"}, // if i <= 0 jump to end
    {OpCode::LOAD, "sum"},
    {OpCode::LOAD, "i"},
    {OpCode::ADD, ""},
    {OpCode::STORE, "sum"},
    {OpCode::LOAD, "i"},
    {OpCode::PUSH, "1"},
    {OpCode::SUB, ""},
    {OpCode::STORE, "i"},
    {OpCode::JUMP, "4"}, // loop back
    {OpCode::LOAD, "sum"},
    {OpCode::PRINT, "1"}, // println sum
    {OpCode::PUSH, "\"terminer\""},
    {OpCode::PRINT_STRING, "1"} // println "terminer"
};

// Exécuter le programme ( 1 + 2 + 3 + 4 + 5 = 15 )

    if (rt.execute(program)) {
        std::cout << "\nExécution terminée sans erreur \n";
        rt.dumpVariables();
        rt.dumpStack();
    } else {
        std::cout << "\nErreur pendant l'exécution \n";
    }
    return 0;
    }