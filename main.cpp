#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "io.hpp"
#include "dfa_utils.hpp"
#include "token.hpp"
#include "ast_printer.hpp"


void run(char *argv[]) {
    const WLangReader reader{argv, "./resources/factorial.wlang"};
    const auto program_text = reader.read_program();

    Lexer lexer{program_text};
    const auto tokens = lexer.tokenize();
    //lexer.print_tokens( tokens );

    Parser parser{tokens};
    auto stmt = parser.parse();


    const auto fv = dfa_utils::free_variables_stmt(stmt);
    for (const auto& element : fv) {
        std::cout << element.name_ << " ";
    }

    const auto bs = dfa_utils::blocks(stmt);
    /*auto visitor = overload {
        [&bs](const Skip& s) {
            std::cout << ""
        },
        [&bs](const Assign& a) {
            bs.insert(a);
        },
        [&bs](const Cond& c) {

        }
    };*/

    for (const auto& element : bs) {
        //std::visit(visitor, element);
        std::cout << "element.pp = " << element.pp_ << "\n";
    }

    std::cout << std::endl;
    //ASTPrinter printer{};
    //printer.print_AST(*stmt);
    //LiveVariableAnalysis lv{stmt};
}

int main(int argc, char *argv[]) {
    run(argv);

    return 0;
}
