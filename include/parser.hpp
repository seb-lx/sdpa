#pragma once

#include <vector>

#include "ast.hpp"
#include "lexer.hpp"
#include "utils.hpp"


class Parser {
    std::vector<Token> tokens;
    Token current_token;
    int index;

    bool is_next_binary_op_relation_operation() noexcept;
    int get_index_of_operator() noexcept;

public:
    explicit Parser(const std::vector<Token> &tokens):
        tokens(tokens), current_token(std::make_pair(TokenType::None, "")), index(0) {}

    Token match(TokenType expected_type);
    void consume();

    [[nodiscard]] std::unique_ptr<Stmt> parse();
    [[nodiscard]] std::unique_ptr<Stmt> parse_statement();
    [[nodiscard]] std::unique_ptr<Stmt> parse_basic_block();
    [[nodiscard]] std::unique_ptr<Cond> parse_basic_block_condition();
    [[nodiscard]] std::unique_ptr<Stmt> parse_skip_statement();
    [[nodiscard]] std::unique_ptr<Stmt> parse_assignment_statement();
    [[nodiscard]] std::unique_ptr<Stmt> parse_if_statement();
    [[nodiscard]] std::unique_ptr<Stmt> parse_while_statement();
    [[nodiscard]] std::unique_ptr<AExp> parse_arithmetic_expression();
    [[nodiscard]] std::unique_ptr<BExp> parse_boolean_expression();
};