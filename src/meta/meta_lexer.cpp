#include "meta/token.hpp"
#include "meta/meta_lexer.hpp"
#include <cctype>
#include <iostream>

namespace meta {

MetaLexer::MetaLexer(const std::string& input)
    : source(input), pos(0), line(1), column(1) {}

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
