#pragma once
#include "token.hpp"
#include <string>
#include <vector>

namespace compilo {

class Lexer {
public:
    explicit Lexer(const std::string& src)
        : source(src), pos(0), line(1), col(1) {}

    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int line, col;

    char peek(int offset = 0) const;
    char advance();
    void skipWhitespaceAndComments();
    Token makeToken(TokenType type, const std::string& value);

    Token identifierOrKeyword();
    Token number();
    Token string();
};

} 
