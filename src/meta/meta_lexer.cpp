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

int MetaLexer::extract_action() {
    skip_whitespace();
    if (peek() == '#') {
        advance(); // skip '#'
        int start = pos;
        while (std::isdigit(peek())) advance();
        if (start < static_cast<int>(pos)) {
            return std::stoi(source.substr(start, pos - start));
        }
    }
    return 0; // default
}

Token MetaLexer::make(TokenType type, const std::string& val, int len, int action) {
    return Token{
        type,
        val.empty() ? source.substr(pos - len, len) : val,
        action,
        line,
        column - len
    };
}

Token MetaLexer::lex_identifier() {
    int start = pos;
    while (std::isalnum(peek()) || peek() == '_') advance();
    std::string val = source.substr(start, pos - start);
    int action = extract_action();
    return Token{TokenType::Identifier, val, action, line, column - static_cast<int>(pos - start)};
}

Token MetaLexer::lex_terminal() {
    advance();

    int start = pos;
    while (peek() != '\'' && peek() != '\0') {
        advance();
    }

    std::string val = source.substr(start, pos - start);

    if (peek() == '\'') {
        advance();
    } else {
        std::cerr << "Erreur : terminal non ferme à la ligne " << line << ", colonne " << column << std::endl;
    }

    int action = extract_action();

    return Token{TokenType::Terminal, val, action, line, column - static_cast<int>(val.size() + 2)};
}

std::vector<Token> MetaLexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < source.size()) {
        skip_whitespace();

        if (peek() == '\'') {
            tokens.push_back(lex_terminal());
        }

        if (match("->")) tokens.push_back(make(TokenType::Arrow, "->", 2, extract_action()));
        else if (match("|")) tokens.push_back(make(TokenType::Pipe, "|", 1, extract_action()));
        else if (match("*")) tokens.push_back(make(TokenType::Star, "*", 1, extract_action()));
        else if (match("+")) tokens.push_back(make(TokenType::Plus, "+", 1, extract_action()));
        else if (match("(/")) tokens.push_back(make(TokenType::SlashOpen, "(/", 2, extract_action()));
        else if (match("/)")) tokens.push_back(make(TokenType::SlashClose, "/)", 2, extract_action()));
        else if (match("(")) tokens.push_back(make(TokenType::LParen, "(", 1, extract_action()));
        else if (match(")")) tokens.push_back(make(TokenType::RParen, ")", 1, extract_action()));
        else if (match("[")) tokens.push_back(make(TokenType::LBracket, "[", 1, extract_action()));
        else if (match("]")) tokens.push_back(make(TokenType::RBracket, "]", 1, extract_action()));
        else if (match(",")) tokens.push_back(make(TokenType::Comma, ",", 1, extract_action()));
        else if (match(".")) tokens.push_back(make(TokenType::Dot, ".", 1, extract_action()));
        else if (match(";")) {
            if (tokens.empty() || tokens.back().type != TokenType::Comma) {
                throw std::runtime_error(
                    "Erreur : ';' doit etre precede d'une virgule (sequence ',;') attendue a la ligne " +
                    std::to_string(line)
                );
            }

            tokens.push_back(make(TokenType::Semicolon, ";", 1, extract_action()));

            skip_whitespace();
            if (pos < source.size()) {
                throw std::runtime_error(
                    "Erreur : contenu trouve apres le ';' final a la ligne " + std::to_string(line)
                );
            }
    tokens.push_back(make(TokenType::EndOfFile, "", 0, 0));
    break;
}
        else if (std::isalpha(peek())) tokens.push_back(lex_identifier());
        else if (peek() == '\0') break;
        else {
            std::cerr << "Unknown character: " << peek() << " at line " << line << ", column " << column << "\n";
            advance(); // skip unknown character
        }
    }

    tokens.push_back(make(TokenType::EndOfFile, "", 0, 0));
    return tokens;
}

} 
