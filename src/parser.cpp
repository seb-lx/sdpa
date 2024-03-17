#include <format>

#include "parser.hpp"


Token Parser::match(TokenType expected_type)
{
    if(current_token.first == expected_type) {
        Token token = current_token;
        Parser::consume();
        return token;
    }
    else {
        throw SyntaxError("Syntax error - invalid token!");
    }
}

void Parser::consume()
{
    index += 1;
    current_token = (index < tokens.size()) ? tokens[index] : std::make_pair(TokenType::None, "");
}

std::unique_ptr<Stmt> Parser::parse()
{
    if(index >= tokens.size()) throw std::runtime_error("Cannot parse empty program!");

    current_token = tokens[index];
    return Parser::parse_statement();
}

std::unique_ptr<Stmt> Parser::parse_statement()
{
    std::unique_ptr<Stmt> left_statement = nullptr;

    if(current_token.first == TokenType::OPEN_BRACKET)
        left_statement = Parser::parse_basic_block();
    else if(current_token.first == TokenType::KEYWORD && current_token.second == "if")
        left_statement = Parser::parse_if_statement();
    else if(current_token.first == TokenType::KEYWORD && current_token.second == "while")
        left_statement = Parser::parse_while_statement();

    // Sequential composition
    if(current_token.first != TokenType::None) {
        if(current_token.first == TokenType::SEMICOLON && index < tokens.size()) {
            Parser::match(TokenType::SEMICOLON);
            std::unique_ptr<Stmt> right_statement = Parser::parse_statement();
            return std::make_unique<SeqComp>(
                std::move(left_statement),
                std::move(right_statement)
            );
        }
    }

    return left_statement;
}

std::unique_ptr<Stmt> Parser::parse_basic_block()
{
    Parser::match(TokenType::OPEN_BRACKET);
    if(current_token.first == TokenType::KEYWORD && current_token.second == "skip")
        return Parser::parse_skip_statement();
    else if(current_token.first == TokenType::VARIABLE)
        return Parser::parse_assignment_statement();
    else
        throw SyntaxError("Unexpected token!");
}

std::unique_ptr<Cond> Parser::parse_basic_block_condition()
{
    Parser::match(TokenType::OPEN_BRACKET);
    std::unique_ptr<BExp> b_exp = std::move(Parser::parse_boolean_expression());
    Parser::match(TokenType::CLOSE_BRACKET);
    Parser::match(TokenType::SUPERSCRIPT);
    std::unique_ptr<PP> pp = std::make_unique<PP>(std::stoul(Parser::match(TokenType::NUMBER).second));
    return std::make_unique<Cond>(std::move(b_exp), std::move(pp));
}

std::unique_ptr<Stmt> Parser::parse_skip_statement()
{
    Parser::match(TokenType::KEYWORD);
    Parser::match(TokenType::CLOSE_BRACKET);
    Parser::match(TokenType::SUPERSCRIPT);
    std::unique_ptr<PP> pp = std::make_unique<PP>(std::stoul(Parser::match(TokenType::NUMBER).second));
    return std::make_unique<Skip>(std::move(pp));
}

std::unique_ptr<Stmt> Parser::parse_assignment_statement()
{
    std::unique_ptr<Var> var =  std::make_unique<Var>(Parser::match(TokenType::VARIABLE).second);
    Parser::match(TokenType::ASSIGN_OP);
    std::unique_ptr<AExp> a_exp = Parser::parse_arithmetic_expression();
    Parser::match(TokenType::CLOSE_BRACKET);
    Parser::match(TokenType::SUPERSCRIPT);
    std::unique_ptr<PP> pp = std::make_unique<PP>(std::stoul(Parser::match(TokenType::NUMBER).second));
    return std::make_unique<Ass>(std::move(var), std::move(a_exp), std::move(pp));
}

std::unique_ptr<Stmt> Parser::parse_if_statement()
{
    Parser::match(TokenType::KEYWORD); // if
    std::unique_ptr<Cond> cond = Parser::parse_basic_block_condition();
    Parser::match(TokenType::KEYWORD); // then
    std::unique_ptr<Stmt> if_branch = Parser::parse_statement();
    Parser::match(TokenType::KEYWORD); // else
    std::unique_ptr<Stmt> else_branch = Parser::parse_statement();
    Parser::match(TokenType::KEYWORD); // fi
    return std::make_unique<If>(std::move(cond), std::move(if_branch), std::move(else_branch));
}

std::unique_ptr<Stmt> Parser::parse_while_statement()
{
    Parser::match(TokenType::KEYWORD); // while
    std::unique_ptr<Cond> cond = Parser::parse_basic_block_condition();
    Parser::match(TokenType::KEYWORD); // do
    std::unique_ptr<Stmt> body = Parser::parse_statement();
    Parser::match(TokenType::KEYWORD); // od
    return std::make_unique<While>(std::move(cond), std::move(body));
}

std::unique_ptr<AExp> Parser::parse_arithmetic_expression()
{
    if(current_token.first == TokenType::VARIABLE)
        return std::make_unique<Var>(Parser::match(TokenType::VARIABLE).second);
    else if(current_token.first == TokenType::NUMBER)
        return std::make_unique<Num>(std::stoul(Parser::match(TokenType::NUMBER).second));
    else if(current_token.first == TokenType::OPEN_PAREN) {
        Parser::match(TokenType::OPEN_PAREN);
        std::unique_ptr<AExp> operand_1 = Parser::parse_arithmetic_expression();
        std::string op_a = Parser::match(TokenType::ARITHMETIC_OP).second;
        std::unique_ptr<AExp> operand_2 = Parser::parse_arithmetic_expression();
        Parser::match(TokenType::CLOSE_PAREN);

        if(op_a == "+")
            return std::make_unique<Add>(std::move(operand_1), std::move(operand_2));
        else if(op_a == "-")
            return std::make_unique<Sub>(std::move(operand_1), std::move(operand_2));
        else if(op_a == "*")
            return std::make_unique<Mul>(std::move(operand_1), std::move(operand_2));
        else
            throw SyntaxError("Unexpected operand!");
    }
    else
        throw SyntaxError("Unexpected token!");
}

std::unique_ptr<BExp> Parser::parse_boolean_expression()
{
    if(current_token.first == TokenType::BOOLEAN) {
        std::string b = Parser::match(TokenType::BOOLEAN).second;
        return (b == "true") ? std::make_unique<Bool>(true) : std::make_unique<Bool>(false);
    }
    else if(current_token.first == TokenType::OPEN_PAREN) {
        Parser::match(TokenType::OPEN_PAREN);

        if(current_token.first == TokenType::KEYWORD && current_token.second == "not") {
            Parser::match(TokenType::KEYWORD);
            std::unique_ptr<BExp> operand = Parser::parse_boolean_expression();
            Parser::match(TokenType::CLOSE_PAREN);
            return std::make_unique<Not>(std::move(operand));
        }

        // Check if boolean expression is either relational or boolean operation
        // Todo: Seems very inefficient for parsing to do it this way?
        if(Parser::is_next_binary_op_relation_operation()) {
            std::unique_ptr<AExp> operand_1 = Parser::parse_arithmetic_expression();
            std::string op_r = Parser::match(TokenType::RELATIONAL_OP).second;
            std::unique_ptr<AExp> operand_2 = Parser::parse_arithmetic_expression();

            Parser::match(TokenType::CLOSE_PAREN);

            if(op_r == "<")
                return std::make_unique<Smaller>(std::move(operand_1), std::move(operand_2));
            else if(op_r == "<=")
                return std::make_unique<SmallerOrEquals>(std::move(operand_1), std::move(operand_2));
            else if(op_r == ">")
                return std::make_unique<Greater>(std::move(operand_1), std::move(operand_2));
            else if(op_r == ">=")
                return std::make_unique<GreaterOrEquals>(std::move(operand_1), std::move(operand_2));
            else
                throw SyntaxError("Unexpected operand!");
        }
        else { // boolean operation
            std::unique_ptr<BExp> operand_1 = Parser::parse_boolean_expression();
            std::string op_b = Parser::match(TokenType::BOOLEAN_OP).second;
            std::unique_ptr<BExp> operand_2 = Parser::parse_boolean_expression();

            Parser::match(TokenType::CLOSE_PAREN);

            if(op_b == "and")
                return std::make_unique<And>(std::move(operand_1), std::move(operand_2));
            else if(op_b == "or")
                return std::make_unique<Or>(std::move(operand_1), std::move(operand_2));
            else
                throw SyntaxError("Unexpected token!");
        }
    }
    else
        throw SyntaxError("Unexpected token!");
}


bool Parser::is_next_binary_op_relation_operation() noexcept
{
    int i = Parser::get_index_of_operator();
    return (tokens[i].first == TokenType::RELATIONAL_OP);
}

int Parser::get_index_of_operator() noexcept
{
    /*
    Finds the index of the operator of the current outer boolean expression.
    E.g.:
        (a op a) gives index of op
        ((a op1 a) op2 a) gives index of op2
        (a op1 (a op2 a)) gives index of op1
    */
    std::vector<std::pair<std::string, int>> stack;
    int i = index;

    while(true) {
        if(tokens[i].first == TokenType::OPEN_PAREN)
            stack.emplace_back(tokens[i].second, i);
        else if(tokens[i].first == TokenType::CLOSE_PAREN)
            stack.pop_back();

        if ((tokens[i].first == TokenType::BOOLEAN_OP or tokens[i].first == TokenType::RELATIONAL_OP) && stack.empty())
            break;
        i += 1;
    }

    return i;
}
