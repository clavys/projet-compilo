#include "compilo/lexer.hpp"
#include <cctype>
#include <iostream>

namespace compilo {

char Lexer::peek(int offset) const {
    if (pos + offset >= source.size()) return '\0';
    return source[pos + offset];
}

char Lexer::advance() {
    char c = peek();
    pos++;
    if (c == '\n') { line++; col = 1; }
    else { col++; }
    return c;
}

void Lexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        if (isspace(c)) { advance(); continue; }
        if (c == '/' && peek(1) == '/') {
            while (c != '\n' && c != '\0') c = advance();
            continue;
        }
        break;
    }
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token{type, value, line, col};
}

Token Lexer::identifierOrKeyword() {
    size_t start = pos;
    while (isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, pos - start);

    if (text == "true") return makeToken(TokenType::True, text);
    if (text == "false") return makeToken(TokenType::False, text);
    if (text == "import") return makeToken(TokenType::Import, text);
    if (text == "var") return makeToken(TokenType::Var, text);
    if (text == "function") return makeToken(TokenType::Function, text);
    if (text == "return") return makeToken(TokenType::Return, text);
    if (text == "if") return makeToken(TokenType::If, text);
    if (text == "else") return makeToken(TokenType::Else, text);
    if (text == "while") return makeToken(TokenType::While, text);
    if (text == "print") return makeToken(TokenType::print, text);
    if (text == "println") return makeToken(TokenType::println, text);
    if (text == "size") return makeToken(TokenType::size, text); 
    if (text == "makeEmpty") return makeToken(TokenType::makeEmpty, text); 
    if (peek() == '[') return makeToken(TokenType::IdentifierList, text); // pour gérer les listes comme des identificateurs]')
    if (peek() == '(') return makeToken(TokenType::IdentifierFonction, text); // pour gérer les appels de fonction comme des identificateurs
    return makeToken(TokenType::Identifier, text);
}

Token Lexer::number() {
    size_t start = pos;
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peek(1))) {
        advance();
        while (isdigit(peek())) advance();
    }
    return makeToken(TokenType::Number, source.substr(start, pos - start));
}

Token Lexer::string() {
    advance(); // skip "
    size_t start = pos;
    while (peek() != '"' && peek() != '\0') advance();
    std::string value = source.substr(start, pos - start);
    advance(); // skip closing "
    return makeToken(TokenType::String, value);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        skipWhitespaceAndComments();
        char c = peek();
        if (c == '\0') break;

        if (isalpha(c) || c == '_') { tokens.push_back(identifierOrKeyword()); continue; }
        if (isdigit(c)) { tokens.push_back(number()); continue; }
        if (c == '"') { tokens.push_back(string()); continue; }

        switch (c) {

                    // === Opérateurs à 2 caractères ===
            case '&':
                if (peek(1) == '&') {
                    tokens.push_back(makeToken(TokenType::And, "&&"));
                    advance(); advance();
                } else {
                    tokens.push_back(makeToken(TokenType::Unknown, "&"));
                    advance();
                }
                break;

            case '|':
                if (peek(1) == '|') {
                    tokens.push_back(makeToken(TokenType::Or, "||"));
                    advance(); advance();
                } else {
                    tokens.push_back(makeToken(TokenType::Unknown, "|"));
                    advance();
                }
                break;

            case '=':
                if (peek(1) == '=') {
                    tokens.push_back(makeToken(TokenType::Equal, "=="));
                    advance(); advance();
                } else {
                    tokens.push_back(makeToken(TokenType::Assign, "="));
                    advance();
                }
                break;

            case '!':
                if (peek(1) == '=') {
                    tokens.push_back(makeToken(TokenType::NotEqual, "!="));
                    advance(); advance();
                } else {
                    tokens.push_back(makeToken(TokenType::Not, "!"));
                    advance();
                }
                break;

            case '<':
                if (peek(1) == '=') {
                    tokens.push_back(makeToken(TokenType::LessEqual, "<="));
                    advance(); advance();
                } else {
                    tokens.push_back(makeToken(TokenType::Less, "<"));
                    advance();
                }
                break;

            case '>':
                if (peek(1) == '=') {
                    tokens.push_back(makeToken(TokenType::GreaterEqual, ">="));
                    advance(); advance();
                } else {
                    tokens.push_back(makeToken(TokenType::Greater, ">"));
                    advance();
                }
                break;

                // === Opérateurs simples ===
            
            case '-':
                if (peek(1) == '>') { tokens.push_back(makeToken(TokenType::Arrow, "->")); advance(); advance(); }
                else { tokens.push_back(makeToken(TokenType::Minus, "-")); advance(); }
                break;
            
            case '*': tokens.push_back(makeToken(TokenType::Star, "*")); advance(); break;
            case '/': tokens.push_back(makeToken(TokenType::Slash, "/")); advance(); break;
            case '%': tokens.push_back(makeToken(TokenType::Percent, "%")); advance(); break;
            case ',': tokens.push_back(makeToken(TokenType::Comma, ",")); advance(); break;
            case ';': tokens.push_back(makeToken(TokenType::Semicolon, ";")); advance(); break;
            case '(': tokens.push_back(makeToken(TokenType::LParen, "(")); advance(); break;
            case ')': tokens.push_back(makeToken(TokenType::RParen, ")")); advance(); break;
            case '{': tokens.push_back(makeToken(TokenType::LBrace, "{")); advance(); break;
            case '}': tokens.push_back(makeToken(TokenType::RBrace, "}")); advance(); break;
            case '[': tokens.push_back(makeToken(TokenType::LBracket, "[")); advance(); break;
            case ']': tokens.push_back(makeToken(TokenType::RBracket, "]")); advance(); break;
            case '+': tokens.push_back(makeToken(TokenType::Plus, "+")); advance(); break;
            case '.': tokens.push_back(makeToken(TokenType::Dot, ".")); advance(); break;
            case ':': tokens.push_back(makeToken(TokenType::Colon, ":")); advance(); break;
            default: advance(); tokens.push_back(makeToken(TokenType::Unknown, std::string(1, c))); break;
        }
    }
    tokens.push_back(makeToken(TokenType::EndOfFile, "EOF"));
    return tokens;
}

} // namespace compilo
