#include "meta/token.hpp"
#include "meta/meta_lexer.hpp"
#include <cctype>
#include <iostream>

namespace meta {

MetaLexer::MetaLexer(const std::string& input)
    : source(input), pos(0), line(1), column(1) {}

std::vector<Token> MetaLexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < source.size()) {
        skip_whitespace();

        if (match("->")) tokens.push_back(make(TokenType::Arrow, "->", 2));
        else if (match("|")) tokens.push_back(make(TokenType::Pipe));
        else if (match("*")) tokens.push_back(make(TokenType::Star));
        else if (match("+")) tokens.push_back(make(TokenType::Plus));
        else if (match("(/")) tokens.push_back(make(TokenType::SlashOpen, "(/", 2));
        else if (match("/)")) tokens.push_back(make(TokenType::SlashClose, "/)", 2));
        else if (match("(")) tokens.push_back(make(TokenType::LParen));
        else if (match(")")) tokens.push_back(make(TokenType::RParen));
        else if (match("[")) tokens.push_back(make(TokenType::LBracket));
        else if (match("]")) tokens.push_back(make(TokenType::RBracket));
        else if (match(";")) tokens.push_back(make(TokenType::Semicolon));
        else if (std::isalpha(peek())) tokens.push_back(lex_identifier());
        else {
            std::cerr << "Unknown character: " << peek() << "\n";
            advance();
        }
    }
    tokens.push_back(make(TokenType::EndOfFile, "", 0));
    return tokens;
}

Token MetaLexer::make(TokenType type, const std::string& val, int len) {
    return Token{type, val.empty() ? source.substr(pos - len, len) : val, line, column - len};
}

char MetaLexer::peek() const {
    return pos < source.size() ? source[pos] : '\0';
}

void MetaLexer::advance() {
    if (peek() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    pos++;
}

bool MetaLexer::match(const std::string& expected) {
    if (source.substr(pos, expected.size()) == expected) {
        pos += expected.size();
        column += expected.size();
        return true;
    }
    return false;
}

void MetaLexer::skip_whitespace() {
    while (std::isspace(peek())) advance();
}

Token MetaLexer::lex_identifier() {
    int start = pos;
    while (std::isalnum(peek()) || peek() == '_') advance();
    return Token{TokenType::Identifier, source.substr(start, pos - start), line, column - static_cast<int>(pos - start)};
}

} 
