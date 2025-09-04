#pragma once
#include "opcode.hpp"
#include <string>

struct Instruction {
    OpCode opcode;
    std::string operand;
    std::string operand2; // pour les fonctions (nombre d'arguments)

        Instruction(OpCode op, const std::string& opd = "", const std::string& opd2 = "")
        : opcode(op), operand(opd), operand2(opd2) {}

};

