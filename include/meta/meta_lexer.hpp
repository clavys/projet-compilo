#pragma once

#include "meta/token.hpp"
#include <vector>
#include <string>

namespace meta {

class MetaLexer {
public:
    MetaLexer(const std::string& input);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int line, column;
    Token make(TokenType type, const std::string& val = "", int len = 1, int action = 0);
    char peek() const;
    void advance();
    bool match(const std::string& expected);
    void skip_whitespace();
    Token lex_identifier();
    int extract_action();
    Token lex_terminal();
};
} 