#include "lexer.hpp"

std::vector<Token> Lexer::tokenize(const std::string &program)
{
    std::vector<Token> tokens;

    // Define regular expressions for different token types
    std::regex regex_pattern(R"((?:skip|if|then|else|fi|while|do|od|not|true|false|\d+|[a-zA-Z_]\w*|<=|>=|[+\-*]|and|or|<|>|:=|;|\(|\)|\[|\]|\^))");
    std::sregex_iterator it(program.begin(), program.end(), regex_pattern);
    std::sregex_iterator end;

    while (it != end) {
        TokenType type;
        std::string value = it->str();
        std::cout << value << " ";

        if (value == "skip" || value == "if" || value == "then" || value == "else" ||
            value == "fi" || value == "while" || value == "do" || value == "od" ||
            value == "not") {
            type = TokenType::KEYWORD;
        } else if (value == "true" || value == "false") {
            type = TokenType::BOOLEAN;
        } else if (std::all_of(value.begin(), value.end(), ::isdigit)) {
            type = TokenType::NUMBER;
        } else if (value == "and" || value == "or") {
                type = TokenType::BOOLEAN_OP;
        } else if (std::isalpha(value[0]) || value[0] == '_') {
            type = TokenType::VARIABLE;
        } else if (value == "+" || value == "-" || value == "*") {
            type = TokenType::ARITHMETIC_OP;
        } else if (value == "<" || value == ">" || value == "<=" || value == ">=") {
            type = TokenType::RELATIONAL_OP;
        } else if (value == ":=") {
            type = TokenType::ASSIGN_OP;
        } else if (value == ";") {
            type = TokenType::SEMICOLON;
        } else if (value == "(") {
            type = TokenType::OPEN_PAREN;
        } else if (value == ")") {
            type = TokenType::CLOSE_PAREN;
        } else if (value == "[") {
            type = TokenType::OPEN_BRACKET;
        } else if (value == "]") {
            type = TokenType::CLOSE_BRACKET;
        } else if (value == "^") {
            type = TokenType::SUPERSCRIPT;
        } else {
            throw std::runtime_error("Unknown token!");
        }

        tokens.emplace_back(type, value);
        ++it;
    }
    std::cout << "\n";

    return tokens;
}
