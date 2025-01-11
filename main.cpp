#include <iostream>

#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "io.hpp"
#include "dfa_utils.hpp"


void run(char *argv[])
{
    const WLangReader reader(argv, "./resources/factorial.wlang");
    const std::string program_text = reader.read_program();

    Lexer lexer(program_text);
    std::vector<Token> tokens = lexer.tokenize();
    //lexer.print_tokens(tokens);

    Parser parser(tokens);
    std::unique_ptr<Stmt> stmt = parser.parse();


    auto fv = dfa_utilities::free_variables_stmt(stmt);
    for (const auto& element : fv) {
        std::cout << element._name << " ";
    }

    std::cout << std::endl;
    //ASTPrinter printer;
    //printer.print_AST(*stmt);
    //auto lv = LiveVariableAnalysis(stmt);
}

int main(int argc, char *argv[])
{
    run(argv);

    return 0;
}
