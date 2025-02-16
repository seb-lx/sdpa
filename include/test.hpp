#pragma once

#include "dfa_utils.hpp"


void testing_dfa_utils(const Stmt* stmt)
{
    const auto fvs = dfa_utils::free_variables_stmt(stmt);
    const auto pps = dfa_utils::program_points(stmt);
    const auto bs = dfa_utils::blocks(stmt);
    const auto b = dfa_utils::get_block(stmt, 6);
    const auto occ = dfa_utils::pp_occurences(stmt, 6);
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
}
