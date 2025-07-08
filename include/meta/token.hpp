#pragma once
#include <string>
#include <ostream>

namespace meta {

enum class TokenType {
    Identifier,      // Ex: N, E, F
    Terminal,        // Ex: ELTER, IDNTER
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
    EndOfFile,       // EOF
    Unknown          // Anything non reconnu
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

inline std::ostream& operator<<(std::ostream& os, const Token& t) {
    return os << "\"" << t.value << "\", " << t.line << ":" << t.column << ")";
              
}

} // namespace meta