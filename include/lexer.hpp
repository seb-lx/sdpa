#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <iostream>


enum class TokenKind {
    Variable,
    Number,

    SkipKeyword,
    IfKeyword,
    ThenKeyword,
    ElseKeyword,
    FiKeyword,
    WhileKeyword,
    DoKeyword,
    OdKeyword,
    TrueKeyword,
    FalseKeyword,
    NotKeyword,

    ArithmeticOperand,
    BooleanOperand,
    RelationalOperand,

    AssignOperand,
    Semicolon,
    OpenParen,
    CloseParen,
    OpenBracket,
    CloseBracket,
    Superscript,

    Invalid,
    EndOfFile
};

const std::unordered_map<std::string, TokenKind> KEYWORD_STRING_TO_KIND_MAP = {
    { "skip",   TokenKind::SkipKeyword },
    { "if",     TokenKind::IfKeyword },
    { "then",   TokenKind::ThenKeyword },
    { "else",   TokenKind::ElseKeyword },
    { "fi",     TokenKind::FiKeyword },
    { "while",  TokenKind::WhileKeyword },
    { "do",     TokenKind::DoKeyword },
    { "od",     TokenKind::OdKeyword },
    { "true",   TokenKind::TrueKeyword },
    { "false",  TokenKind::FalseKeyword },
    { "not",    TokenKind::NotKeyword },
};

const std::unordered_map<TokenKind, std::string> KIND_TO_STRING_MAP = {
    { TokenKind::Variable,           "Variable" },
    { TokenKind::Number,             "Number" },

    { TokenKind::SkipKeyword,        "skip" },
    { TokenKind::IfKeyword,          "if" },
    { TokenKind::ThenKeyword,        "then" },
    { TokenKind::ElseKeyword,        "else" },
    { TokenKind::FiKeyword,          "fi" },
    { TokenKind::WhileKeyword,       "while" },
    { TokenKind::DoKeyword,          "do" },
    { TokenKind::OdKeyword,          "od" },
    { TokenKind::TrueKeyword,        "true" },
    { TokenKind::FalseKeyword,       "false" },
    { TokenKind::NotKeyword,         "not" },

    { TokenKind::ArithmeticOperand,  "ArithmeticOperand" },
    { TokenKind::BooleanOperand,     "BooleanOperand" },
    { TokenKind::RelationalOperand,  "RelationalOperand" },

    { TokenKind::AssignOperand,      "AssignOperand" },
    { TokenKind::Semicolon,          "Semicolon" },
    { TokenKind::OpenParen,          "OpenParen" },
    { TokenKind::CloseParen,         "CloseParen" },
    { TokenKind::OpenBracket,        "OpenBracket" },
    { TokenKind::CloseBracket,       "CloseBracket" },
    { TokenKind::Superscript,        "Superscript" },

    { TokenKind::Invalid,            "Invalid" },
    { TokenKind::EndOfFile,          "EndOfFile" }
};


using Token = std::pair<TokenKind, std::string>;

class Lexer {
public:
    explicit Lexer(std::string);
    [[nodiscard]] std::vector<Token> tokenize();
    void print_tokens(const std::vector<Token>&) const;

private:
    std::string _program_text;
    unsigned int _position;

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
