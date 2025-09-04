#pragma once
#include <string>
#include <ostream>

namespace compilo {

enum class TokenType {
    // === Mots-clés ===
    Import,
    Var,
    Function,
    Return,
    print,
    println,
    makeEmpty,

    // === fonctions sur les listes ===
    size,

    // === Identificateurs et littéraux ===
    Identifier,
    IdentifierList, 
    IdentifierFonction,
    Number,
    String,
    True,
    False,

    // === Opérateurs logiques ===
    And,        // &&
    Or,         // ||
    Not,        // !
    Equal,      // ==
    NotEqual,   // !=

    // === Opérateurs de contrôle de flux ===
    If,        // if
    Else,       // else
    While,      // while

    // === Opérateurs relationnels ===
    Less,       // <
    Greater,    // >
    LessEqual,  // <=
    GreaterEqual, // >=

    // === Opérateurs arithmétiques ===
    Plus,       // +
    Minus,      // -
    Star,       // *
    Slash,      // /
    Percent,    // %

    // === Symboles divers ===
    Assign,     // =
    Arrow,      // ->
    Comma,      // ,
    Semicolon,  // ;
    LParen,     // (
    RParen,     // )
    LBrace,     // {
    RBrace,     // }
    LBracket,   // [
    RBracket,   // ]
    Dot,        // .
    Colon,      // :

    // Fin
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

inline std::ostream& operator<<(std::ostream& os, const Token& t) {
    return os << "[" << static_cast<int>(t.type) 
              << "] \"" << t.value << "\" at " 
              << t.line << ":" << t.column;
}

}
