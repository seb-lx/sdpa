#pragma once

#include <vector>
#include <string>
#include <utility>
#include <iostream>

#include "token.hpp"


class Lexer {
public:
    explicit Lexer(std::string);

	Lexer(const Lexer&) = delete;
	Lexer(Lexer&&) = delete;
	auto operator=(const Lexer&) -> Lexer& = delete;
	auto operator=(Lexer&&) -> Lexer& = delete; 

    [[nodiscard]] std::vector<Token> tokenize();
    void print_tokens(const std::vector<Token>&) const;

private:
    std::string program_text_;
    unsigned int position_;

    void advance();
    void skip_whitespace();
    bool has_next();
    Token next_token();

    [[nodiscard]] Token process_number();
    [[nodiscard]] Token process_keyword_or_variable();
    [[nodiscard]] Token process_assign();
    [[nodiscard]] Token process_relational_l();
    [[nodiscard]] Token process_relational_g();
};
