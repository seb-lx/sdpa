#pragma once

#include <cassert>
#include <algorithm>

#include "analysis_utils.hpp"
#include "set_utils.hpp"


namespace WLangTestPrograms {
    extern std::shared_ptr<Stmt> factorial_stmt;
}

void print_test_result(bool b, const std::string &func_name);

void test_free_variables_a_exp_trivial(bool print);

void test_free_variables_a_exp(bool print);

void test_free_variables_a_exp_complex(bool print);

void test_free_variables_stmt_complex(bool print);

void test_program_points_complex(bool print);

void test_is_program_well_formed_complex(bool print);

void test_program_point_occurrences(bool print);

void test_elementary_blocks(bool print);

void test_get_elementary_block(bool print);

void test_initial_program_point(bool print);

void test_is_program_point_initial(bool print);

void test_final_program_points(bool print);

void test_is_program_point_final(bool print);

void test_control_flow(bool print);

void test_has_isolated_entries(bool print);

void test_has_isolated_exits(bool print);
