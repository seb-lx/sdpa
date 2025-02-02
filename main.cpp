#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "io.hpp"
#include "utils.hpp"
#include "dfa_utils.hpp"
#include "token.hpp"
#include "ast_printer.hpp"
#include "lv.hpp"


void run(char *argv[]) {
    const WLangReader reader{argv, "./resources/ex1_4_b.wlang"};
    const auto program_text = reader.read_program();

    Lexer lexer { program_text };
    const auto tokens = lexer.tokenize();
    //lexer.print_tokens( tokens );

    Parser parser{tokens};
    const auto stmt = parser.parse();

    // dfa_utils tests
    const auto fvs = dfa_utils::free_variables_stmt(stmt.get());
    const auto pps = dfa_utils::program_points(stmt.get());
    const auto bs = dfa_utils::blocks(stmt.get());
    const auto b = dfa_utils::get_block(stmt.get(), 6);
    const auto occ = dfa_utils::pp_occurences(stmt.get(), 6);
    const auto wf = dfa_utils::well_formed(stmt.get());
    const auto init = dfa_utils::initial_pp(stmt.get());
    const auto final = dfa_utils::final_pps(stmt.get());
    const auto cf = dfa_utils::control_flow(stmt.get());
    const auto ientries = dfa_utils::has_isolated_entries(stmt.get());
    const auto iexits = dfa_utils::has_isolated_exits(stmt.get());
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

    //ASTPrinter printer{};
    //printer.print_AST(*stmt);

    LiveVariableAnalysis lva{ stmt.get() }; // need to be in same scope as stmt var due to internal raw ptr
    auto lvs = lva.compute();
    LiveVariableAnalysis::print_result(lvs);
}

int main(int argc, char *argv[]) {
    run(argv);

    return 0;
}
