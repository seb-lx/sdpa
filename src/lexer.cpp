#include "lexer.hpp"


Lexer::Lexer(std::string program_text) :
    program_text_{std::move(program_text)}, position_{0} {}

void Lexer::advance() {
    ++position_;
}

void Lexer::skip_whitespace() {
    if (position_ >= program_text_.size()) {
        return;
    }

    while (std::isspace(program_text_[position_])) {
        advance();
    }
}

bool Lexer::has_next() {
    return position_ < program_text_.size();
}

Token Lexer::next_token() {
    skip_whitespace();

    if (has_next()) {
        const char c = program_text_[position_];

        if (std::isdigit(c)) {
            return process_number();
        }
        else if (std::isalpha(c)) {
            return process_keyword_or_variable();
        }
        else if (c == ':') {
            return process_assign();
        }
        else if (c == '<') {
            return process_relational_l();
        }
        else if (c == '>') {
            return process_relational_g();
        }
        else if (c == '+') {
            advance();
            return {TokenKind::ArithmeticOperand, std::string(1, c)};
        }
        else if (c == '-') {
            advance();
            return {TokenKind::ArithmeticOperand, std::string(1, c)};
        }
        else if (c == '*') {
            advance();
            return {TokenKind::ArithmeticOperand, std::string(1, c)};
        }
        else if (c == '^') {
            advance();
            return {TokenKind::Superscript, std::string(1, c)};
        }
        else if (c == ';') {
            advance();
            return {TokenKind::Semicolon, std::string(1, c)};
        }
        else if (c == '(') {
            advance();
            return {TokenKind::OpenParen, std::string(1, c)};
        }
        else if (c == ')') {
            advance();
            return {TokenKind::CloseParen, std::string(1, c)};
        }
        else if (c == '[') {
            advance();
            return {TokenKind::OpenBracket, std::string(1, c)};
        }
        else if (c == ']') {
            advance();
            return {TokenKind::CloseBracket, std::string(1, c)};
        }
    }

    return {TokenKind::EndOfFile, ""};
}

Token Lexer::process_number() {
    std::string number{};

    while (has_next() && std::isdigit(program_text_[position_])) {
        number += program_text_[position_];
        advance();
    }

    return {TokenKind::Number, number};
}

Token Lexer::process_keyword_or_variable() {
    std::string s{};

    while (has_next() && std::isalnum(program_text_[position_])) {
        s += program_text_[position_];
        advance();
    }

    // Retrieve keyword
    auto it = KEYWORD_STRING_TO_KIND_MAP.find(s);
    if (it != KEYWORD_STRING_TO_KIND_MAP.end()) {
        return {it->second, s};
    }

    // No keyword found, therefore variable
    return {TokenKind::Variable, s};
}

Token Lexer::process_assign() {
    advance();

    if (has_next() && program_text_[position_] == '=') {
        advance();
        return {TokenKind::AssignOperand, ":="};
    }

    throw std::invalid_argument("Invalid assignment operator");
}

Token Lexer::process_relational_l() {
    advance();

    if (has_next() && program_text_[position_] == '=') {
        advance();
        return {TokenKind::RelationalOperand, "<="};
    }

    return {TokenKind::RelationalOperand, "<"};
}

Token Lexer::process_relational_g() {
    advance();

    if (has_next() && program_text_[position_] == '=') {
        advance();
        return {TokenKind::RelationalOperand, ">="};
    }

    return {TokenKind::RelationalOperand, ">"};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens{};

    Token current_token = next_token();
    while (current_token.first != TokenKind::EndOfFile) {
        tokens.push_back(current_token);
        current_token = next_token();
    }

    return tokens;
}


void Lexer::print_tokens(const std::vector<Token>& tokens) const {
    std::cout << "==================== LEXER ====================\n";
    for (const auto& token: tokens) {
        if (token.first == TokenKind::Semicolon) {
            std::cout << KIND_TO_STRING_MAP.at(token.first) << "( \"" << token.second << "\" )\n";
        }
        else {
            std::cout << KIND_TO_STRING_MAP.at(token.first) << "( \"" << token.second << "\" ) ";
        }
    }
    std::cout << "\n===============================================\n";
}
