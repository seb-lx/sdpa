#pragma once

#include <set>
#include <algorithm>
#include <utility>

#include "utils.hpp"
#include "set_utils.hpp"
#include "ast.hpp"
#include "ast_printer.hpp"


namespace dfa_utils {
    // dfa utility functions
    FreeVariables free_variables_aexp(const AExp* aexp);
    FreeVariables free_variables_bexp(const BExp* bexp);
    FreeVariables free_variables_stmt(const Stmt* stmt);

    std::set<PP> program_points(const Stmt* stmt);

    /**
     * Returns the elementary block for the given program point.
     * If the program point is not found within the stmt, return nullptr
     */
    const Block* get_block(const Stmt* stmt, const PP pp);
    ElementaryBlocks blocks(const Stmt* stmt);

    bool well_formed(const Stmt* stmt);
    unsigned int pp_occurences(const Stmt* stmt, const PP pp);
    PP initial_pp(const Stmt* stmt);
    bool is_initial_pp(const Stmt* stmt, const PP pp);
    std::set<PP> final_pps(const Stmt* stmt);
    bool is_final_pp(const Stmt* stmt, const PP pp);

    CFG control_flow(const Stmt* stmt);

    bool has_isolated_entries(const Stmt* stmt);
    bool has_isolated_exits(const Stmt* stmt);


    namespace io {
        // Printer methods
        void print_var_set(const FreeVariables& vars);
        void print_pp_set(const std::set<PP>& pps);
        void print_block_set(const ElementaryBlocks& blocks);
        void print_cf_set(const CFG& cf);
    }
}
