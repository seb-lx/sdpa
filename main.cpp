#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "io.hpp"
#include "utils.hpp"
#include "token.hpp"
#include "ast_printer.hpp"
#include "lv.hpp"
#include "test.hpp"


void run(char *argv[]) {
    const WLangReader reader{argv, "./resources/factorial.wlang"};
    const auto program_text = reader.read_program();

    Lexer lexer { program_text };
    const auto tokens = lexer.tokenize();
    //lexer.print_tokens( tokens );

    Parser parser{tokens};
    const auto stmt = parser.parse();

    testing_dfa_utils(stmt.get());

    //ASTPrinter printer{};
    //printer.print_AST(*stmt);

    LiveVariableAnalysis lv{ stmt.get() };
    auto lvs = lv.compute();
    LiveVariableAnalysis::print_result(lvs);
}

int main(int argc, char *argv[]) {
    run(argv);

    return 0;
}
