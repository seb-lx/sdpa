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


    const auto fvs = dfa_utils::free_variables_stmt(stmt);
    const auto pps = dfa_utils::program_points(stmt);
    const auto bs = dfa_utils::blocks(stmt);
    dfa_utils::io::print_var_set(fvs);
    dfa_utils::io::print_pp_set(pps);
    dfa_utils::io::print_block_set(bs);

    //ASTPrinter printer{};
    //printer.print_AST(*stmt);
    //LiveVariableAnalysis lv{stmt};
}

int main(int argc, char *argv[]) {
    run(argv);

    return 0;
}
