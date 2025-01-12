#include "parser.hpp"


Parser::Parser(std::vector<Token> tokens) :
    tokens_(std::move(tokens)),
    current_token_(std::make_pair(TokenKind::Invalid, "")),
    position_(0) {}


Token Parser::match(TokenKind token_kind) {
    const auto token = current_token_;

    if (token.first == token_kind) {
        consume();
        return token;
    }

    throw SyntaxError("Unexpected token (" + KIND_TO_STRING_MAP.at(token.first) + ")!");
}

void Parser::consume() {
    ++position_;
    current_token_ = (position_ < tokens_.size()) ? tokens_[position_] : std::make_pair(TokenKind::EndOfFile, "");
}

std::unique_ptr<Stmt> Parser::parse() {
    if (position_ >= tokens_.size()) throw SyntaxError("Cannot parse empty program!");

    current_token_ = tokens_[position_];
    return parse_statement();
}

unsigned int Parser::parse_program_point() {
    const auto number_lexeme = match(TokenKind::Number).second;
    return std::stoul(number_lexeme);
}

std::unique_ptr<Stmt> Parser::parse_statement() {
    std::unique_ptr<Stmt> left_statement = nullptr;

    const auto& kind = current_token_.first;
    if (kind == TokenKind::OpenBracket) {
        left_statement = parse_skip_or_assign_statement();
    }
    else if (kind == TokenKind::IfKeyword) {
        left_statement = parse_if_statement();
    }
    else if (kind == TokenKind::WhileKeyword) {
        left_statement = parse_while_statement();
    } else {
        throw SyntaxError("Expected skip, assignment, if, or while.");
    }

    // Sequential composition
    if (kind == TokenKind::Semicolon) {
        match(TokenKind::Semicolon);
        auto right_statement = parse_statement();

        return std::make_unique<Stmt>(
            SeqComp {
                std::move(left_statement),
                std::move(right_statement)
            }
        );
    }
        
    return left_statement;
}

std::unique_ptr<Stmt> Parser::parse_skip_or_assign_statement() {
    match(TokenKind::OpenBracket);

    const auto& kind = current_token_.first;
    if (kind == TokenKind::SkipKeyword) {
        return parse_skip_statement();
    }
    else if (kind == TokenKind::Variable) {
        return parse_assign_statement();
    }
    
    throw SyntaxError("Expected skip or assign statement!");
}

std::unique_ptr<Stmt> Parser::parse_skip_statement() {
    match(TokenKind::SkipKeyword);
    match(TokenKind::CloseBracket);
    match(TokenKind::Superscript);
    const PP pp = parse_program_point();

    return std::make_unique<Stmt>(
        Skip { pp }
    );
}

std::unique_ptr<Stmt> Parser::parse_assign_statement() {
    auto var = std::make_unique<Var>(Parser::match(TokenKind::Variable).second);
    match(TokenKind::AssignOperand);
    auto aexp = parse_arithmetic_expression();
    match(TokenKind::CloseBracket);
    match(TokenKind::Superscript);
    const PP pp = parse_program_point();
    
    return std::make_unique<Stmt>(
        Assign {
            std::move(var),
            std::move(aexp),
            pp
        }
    );
}

std::unique_ptr<Stmt> Parser::parse_if_statement() {
    match(TokenKind::IfKeyword);

    match(TokenKind::OpenBracket);
    auto cond = parse_boolean_expression();
    match(TokenKind::CloseBracket);
    match(TokenKind::Superscript);
    const PP pp = parse_program_point();

    match(TokenKind::ThenKeyword);
    auto then_branch = parse_statement();

    match(TokenKind::ElseKeyword);
    auto else_branch = parse_statement();

    match(TokenKind::FiKeyword); 

    return std::make_unique<Stmt>(
        If {
            std::move(cond),
            pp,
            std::move(then_branch),
            std::move(else_branch)
        }
    );
}

std::unique_ptr<Stmt> Parser::parse_while_statement() {
    match(TokenKind::WhileKeyword);

    match(TokenKind::OpenBracket);
    auto cond = parse_boolean_expression();
    match(TokenKind::CloseBracket);
    match(TokenKind::Superscript);
    const PP pp = parse_program_point();

    match(TokenKind::DoKeyword);
    auto body = parse_statement();
    match(TokenKind::OdKeyword);

    return std::make_unique<Stmt>(
        While {
            std::move(cond),
            pp,
            std::move(body)
        }
    );
}

std::unique_ptr<AExp> Parser::parse_arithmetic_expression() {
    const auto& kind = current_token_.first;

    if (kind == TokenKind::Variable) {
        return parse_variable();
    }
    else if (kind == TokenKind::Number) {
        return parse_number();
    }
    else if (kind == TokenKind::OpenParen) {
        return parse_arithmetic_operation();
    }
    else {
        throw SyntaxError("Expected variable, number, or arithmetic expression!");
    }
}

std::unique_ptr<AExp> Parser::parse_variable() {
    return std::make_unique<AExp>(
        Var { match(TokenKind::Variable).second }
    );
}

std::unique_ptr<AExp> Parser::parse_number() {
    const unsigned int number = std::stoul(match(TokenKind::Number).second);

    return std::make_unique<AExp>(
        Num { number }
    );
}

std::unique_ptr<AExp> Parser::parse_arithmetic_operation() {
    match(TokenKind::OpenParen);
    auto lhs = parse_arithmetic_expression();
    const auto op = match(TokenKind::ArithmeticOperand).second;
    auto rhs = parse_arithmetic_expression();
    match(TokenKind::CloseParen);

    return std::make_unique<AExp>(
        ArithmeticOp {
            std::move(lhs),
            op,
            std::move(rhs)
        }
    );
}

std::unique_ptr<BExp> Parser::parse_boolean_expression() {
    const auto& kind = current_token_.first;

    if (kind == TokenKind::TrueKeyword) {
        return parse_true();
    }
    else if (kind == TokenKind::FalseKeyword) {
        return parse_false();
    }
    else if (kind == TokenKind::OpenParen) {
        match(TokenKind::OpenParen);

        if (kind == TokenKind::NotKeyword) {
            return parse_not();
        }

        // Check if boolean expression is either relational or boolean operation
        // Note: is this the only way when modeling opB and opR separately?
        if(is_next_binary_op_opr()) {
            return parse_relational_operation();
        }
        else {
            return parse_boolean_operation();
        }
    }
    else {
        throw SyntaxError("Expected true, false, not, boolean or relational expression!");
    }
}

std::unique_ptr<BExp> Parser::parse_true() {
    return std::make_unique<BExp>(
        True {}
    );
}

std::unique_ptr<BExp> Parser::parse_false() {
    return std::make_unique<BExp>(
        False {}
    );   
}

std::unique_ptr<BExp> Parser::parse_not() {
    match(TokenKind::NotKeyword);
    auto b = parse_boolean_expression();
    match(TokenKind::CloseParen);

    return std::make_unique<BExp>(
        Not { std::move(b) }
    );    
}

std::unique_ptr<BExp> Parser::parse_relational_operation() {
    auto lhs = parse_arithmetic_expression();
    const auto op = match(TokenKind::RelationalOperand).second;
    auto rhs = parse_arithmetic_expression();

    match(TokenKind::CloseParen);

    return std::make_unique<BExp>(
        RelationalOp {
            std::move(lhs),
            op,
            std::move(rhs)
        }
    );
}

std::unique_ptr<BExp> Parser::parse_boolean_operation() {
    auto lhs = parse_boolean_expression();
    const auto op = match(TokenKind::BooleanOperand).second;
    auto rhs = parse_boolean_expression();

    match(TokenKind::CloseParen);

    return std::make_unique<BExp>(
        BooleanOp {
            std::move(lhs),
            op,
            std::move(rhs)
        }
    );
}

bool Parser::is_next_binary_op_opr() noexcept {
    const unsigned int idx = get_idx_of_next_op();
    const auto& kindAtIdx = tokens_[idx].first;
    return kindAtIdx == TokenKind::RelationalOperand;
}

unsigned int Parser::get_idx_of_next_op() noexcept {
    // Stack-based search of index of the next operator.
    //      Note that first open paren already matched.
    //
    // Examples
    //    (a op a) gives index of op
    //    ((a op1 a) op2 a) gives index of op2
    //    (a op1 (a op2 a)) gives index of op1

    std::vector<int> stack;
    unsigned int idx = position_;

    while (true) {
        if (tokens_[idx].first == TokenKind::OpenParen) {
            stack.emplace_back(idx);
        }
        else if (tokens_[idx].first == TokenKind::CloseParen) {
            stack.pop_back();
        }

        // Check if the stack is empty (and we found an opb or opr),
        //  if it is the next operator is from the outer boolean expression
        // This works since the first paren of the outer boolean expression is not on the stack
        //  if this would be the case then we had to check if stack.size() == 1
        const auto& kindAtIdx = tokens_[idx].first;
        bool found_opr_or_opb = kindAtIdx == TokenKind::BooleanOperand or kindAtIdx == TokenKind::RelationalOperand;
        if (stack.empty() && found_opr_or_opb)
            break;

        ++idx;
    }

    return idx;
}
