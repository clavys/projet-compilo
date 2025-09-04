#pragma once
#include <string>
#include <unordered_map>

enum class OpCode {
    IMPORT_MODULE,
    PUSH,
    PUSH_BOOL,
    LOAD,LOAD_LOCAL,
    STORE,STORE_LOCAL,
    ADD, SUB, MUL, DIV, MOD, NEG,
    AND, OR, NOT,
    EQ, NEQ, LT, GT, LE, GE,
    JUMP, JUMP_IF_FALSE,
    MAKE_LIST, LIST_GET, LIST_SET,LIST_LEN,MAKE_EMPTY_LIST,

    PRINT_STRING, PRINT, PRINT_BOOL,
    CALL, FUNC_DECL,RETURN,END_FUNC,FUNC_END,DECLARE_PARAM,PUSH_PARAM,
    PUSH_FRAME,RETURN_VOID,DECLARE_VAR,POP_FRAME,ARG_COUNT,
    
    
    INVALID
};


inline std::string opCodeToString(OpCode op) {
    switch (op) {
        case OpCode::IMPORT_MODULE: return "IMPORT_MODULE";
        case OpCode::PUSH: return "PUSH";
        case OpCode::PUSH_BOOL: return "PUSH_BOOL";
        case OpCode::PUSH_PARAM: return "PUSH_PARAM";
        case OpCode::LOAD: return "LOAD";
        case OpCode::LOAD_LOCAL: return "LOAD_LOCAL";
        case OpCode::STORE: return "STORE";
        case OpCode::STORE_LOCAL: return "STORE_LOCAL";
        case OpCode::ADD: return "ADD";
        case OpCode::SUB: return "SUB";
        case OpCode::MUL: return "MUL";
        case OpCode::DIV: return "DIV";
        case OpCode::MOD: return "MOD";
        case OpCode::NEG: return "NEG";
        case OpCode::AND: return "AND";
        case OpCode::OR: return "OR";
        case OpCode::NOT: return "NOT";
        case OpCode::EQ: return "EQ";
        case OpCode::NEQ: return "NEQ";
        case OpCode::LT: return "LT";
        case OpCode::GT: return "GT";
        case OpCode::LE: return "LE";
        case OpCode::GE: return "GE";
        case OpCode::JUMP: return "JUMP";
        case OpCode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OpCode::MAKE_LIST: return "MAKE_LIST";
        case OpCode::LIST_GET: return "LIST_GET";
        case OpCode::LIST_SET: return "LIST_SET";
        case OpCode::LIST_LEN: return "LIST_LEN";
        case OpCode::MAKE_EMPTY_LIST: return "MAKE_EMPTY_LIST";
        case OpCode::PRINT_STRING: return "PRINT_STRING";
        case OpCode::PRINT: return "PRINT";
        case OpCode::PRINT_BOOL: return "PRINT_BOOL";
        case OpCode::CALL: return "CALL";
        case OpCode::FUNC_DECL: return "FUNC_DECL";
        case OpCode::DECLARE_PARAM: return "DECLARE_PARAM";
        case OpCode::PUSH_FRAME: return "PUSH_FRAME";
        case OpCode::RETURN_VOID: return "RETURN_VOID";
        case OpCode::DECLARE_VAR: return "DECLARE_VAR";
        case OpCode::POP_FRAME: return "POP_FRAME";
        case OpCode::FUNC_END: return "FUNC_END";
        case OpCode::RETURN: return "RETURN";
        case OpCode::END_FUNC: return "END_FUNC";
        case OpCode::ARG_COUNT: return "ARG_COUNT";
        default: return "INVALID";
    }
}