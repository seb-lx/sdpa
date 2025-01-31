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
    const auto b = dfa_utils::get_block(stmt, 7);
    const auto occ = dfa_utils::pp_occurences(stmt, 7);
    const auto wf = dfa_utils::well_formed(stmt);
    const auto init = dfa_utils::initial_pp(stmt);
    const auto final = dfa_utils::final_pps(stmt);
    const auto cf = dfa_utils::control_flow(stmt);
    const auto ientries = dfa_utils::has_isolated_entries(stmt);
    const auto iexits = dfa_utils::has_isolated_exits(stmt);
    dfa_utils::io::print_var_set(fvs);
    dfa_utils::io::print_pp_set(pps);
    dfa_utils::io::print_block_set(bs);
    dfa_utils::io::print_block_set({b});
    std::cout << (wf ? "well formed" : "not well formed") << "\n";
    std::cout << "PP occurences: " << occ << "\n";
    dfa_utils::io::print_pp_set({init});
    dfa_utils::io::print_pp_set(final);
    dfa_utils::io::print_cf_set(cf);
    std::cout << (ientries ? "isolated entries" : "no isolated entries") << "\n";
    std::cout << (iexits ? "isolated exits" : "no isolated exits") << "\n";

    // todo: test , well formedness, init, final, isolated entries, isolated exits

    //ASTPrinter printer{};
    //printer.print_AST(*stmt);
    //LiveVariableAnalysis lv{stmt};
}

int main(int argc, char *argv[]) {
    run(argv);

    return 0;
}
