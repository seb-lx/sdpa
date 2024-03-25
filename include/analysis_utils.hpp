#pragma once

#include <algorithm>
#include <iterator>
#include <set>

#include "ast.hpp"


[[nodiscard]] std::set<Var> free_variables_a_exp(const std::shared_ptr<AExp> &a_exp);
[[nodiscard]] std::set<Var> free_variables_b_exp(const std::shared_ptr<BExp> &b_exp);
[[nodiscard]] std::set<Var> free_variables_stmt(const std::shared_ptr<Stmt> &stmt);
[[nodiscard]] std::set<PP> program_points(const std::shared_ptr<Stmt> &stmt);
[[nodiscard]] bool is_program_well_formed(const std::shared_ptr<Stmt> &stmt);
[[nodiscard]] unsigned int program_point_occurrences(const std::shared_ptr<Stmt> &stmt, const PP &pp);
[[nodiscard]] std::set<std::shared_ptr<Block>> elementary_blocks(const std::shared_ptr<Stmt> &stmt);
[[nodiscard]] std::shared_ptr<Block> get_elementary_block(const std::shared_ptr<Stmt> &stmt, const PP &pp);
[[nodiscard]] PP initial_program_point(const std::shared_ptr<Stmt> &stmt);
[[nodiscard]] std::set<PP> final_program_points(const std::shared_ptr<Stmt> &stmt);
[[nodiscard]] bool is_program_point_initial(const std::shared_ptr<Stmt> &stmt, const PP &pp);
[[nodiscard]] bool is_program_point_final(const std::shared_ptr<Stmt> &stmt, const PP &pp);
[[nodiscard]] std::set<std::pair<PP, PP>> control_flow(const std::shared_ptr<Stmt> &stmt);
[[nodiscard]] bool has_isolated_entries(const std::shared_ptr<Stmt> &stmt);
[[nodiscard]] bool has_isolated_exits(const std::shared_ptr<Stmt> &stmt);
