#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <stdexcept>

#include "ast.hpp"
#include "token.hpp"


class SyntaxError: public std::exception {
private:
    std::string msg;
public:
    explicit SyntaxError(std::string msg): msg(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return msg.c_str();
    }
};


class Parser {
public:
    explicit Parser(std::vector<Token>);

    [[nodiscard]] std::unique_ptr<Stmt> parse();

private:
    std::vector<Token> _tokens;
    Token _current_token;
    unsigned int _position;

    Token match(TokenKind token_kind);
    void consume();

    [[nodiscard]] unsigned int parse_program_point();

    [[nodiscard]] std::unique_ptr<Stmt> parse_statement();
    [[nodiscard]] std::unique_ptr<Stmt> parse_skip_or_assign_statement();
    [[nodiscard]] std::unique_ptr<Stmt> parse_skip_statement();
    [[nodiscard]] std::unique_ptr<Stmt> parse_assign_statement();
    [[nodiscard]] std::unique_ptr<Stmt> parse_if_statement();
    [[nodiscard]] std::unique_ptr<Stmt> parse_while_statement();

    [[nodiscard]] std::unique_ptr<AExp> parse_arithmetic_expression();
    [[nodiscard]] std::unique_ptr<AExp> parse_variable();
    [[nodiscard]] std::unique_ptr<AExp> parse_number();
    [[nodiscard]] std::unique_ptr<AExp> parse_arithmetic_operation();

    [[nodiscard]] std::unique_ptr<BExp> parse_boolean_expression();
    [[nodiscard]] std::unique_ptr<BExp> parse_true();
    [[nodiscard]] std::unique_ptr<BExp> parse_false();
    [[nodiscard]] std::unique_ptr<BExp> parse_not();
    [[nodiscard]] std::unique_ptr<BExp> parse_relational_operation();
    [[nodiscard]] std::unique_ptr<BExp> parse_boolean_operation();
    
    [[nodiscard]] unsigned int get_idx_of_next_op() noexcept;
    [[nodiscard]] bool is_next_binary_op_opr() noexcept;
};
