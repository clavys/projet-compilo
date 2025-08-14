#pragma once
#include <string>
#include <ostream>

namespace meta {

enum class TokenType {
    Identifier,      // Ex: N, E1, a, b, etc.
    Terminal,        // Ex: 'ELTER', 'IDNTER', 'a1', 'b2', etc.
    Arrow,           // ->
    Pipe,            // |
    Star,            // *
    Plus,            // +
    LParen,          // (
    RParen,          // )
    LBracket,        // [
    RBracket,        // ]
    SlashOpen,       // (/
    SlashClose,      // /)
    Comma,           // ,
    Semicolon,       // ;
    Dot,             // .
    EndOfFile,       // EOF
    Unknown          // Anything non reconnu
};

struct Token {
    TokenType type;
    std::string value;
    int action; // Code d'action associé au token
    int line;
    int column;
};

inline std::ostream& operator<<(std::ostream& os, const Token& t) {
    return os << "\"" << t.value << "\", actionCode : " <<t.action<< " , "<<t.line << ":" << t.column << "";
              
}

}