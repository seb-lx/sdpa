#pragma once

#include <algorithm>
#include <iterator>
#include <set>

#include "ast.hpp"
#include "set_utils.hpp"


/*
 * Compute the set of free variables in the given arithmetic expression.
 */
[[nodiscard]] std::set<Var> free_variables_a_exp(const std::shared_ptr<AExp> &a_exp);

/*
 * Compute the set of free variables in the given boolean expression.
 */
[[nodiscard]] std::set<Var> free_variables_b_exp(const std::shared_ptr<BExp> &b_exp);

/*
 * Compute the set of free variables in the given program/statement.
 */
[[nodiscard]] std::set<Var> free_variables_stmt(const std::shared_ptr<Stmt> &stmt);

/*
 * Compute the set of program points within a program/statement.
 */
[[nodiscard]] std::set<PP> program_points(const std::shared_ptr<Stmt> &stmt);

/*
 * Check whether a program/statement is well-formed, i.e. each program point occurs at most once.
 */
[[nodiscard]] bool is_program_well_formed(const std::shared_ptr<Stmt> &stmt);

/*
 * Compute the number of occurrences of the program point in the program/statement.
 */
[[nodiscard]] unsigned int program_point_occurrences(const std::shared_ptr<Stmt> &stmt, const PP &pp);

/*
 * Compute set of elementary blocks in given program/statement.
 */
[[nodiscard]] std::set<std::shared_ptr<Block>> elementary_blocks(const std::shared_ptr<Stmt> &stmt);

/*
 * Return the respective block of a given stmt at pp.
 */
[[nodiscard]] std::shared_ptr<Block> get_elementary_block(const std::shared_ptr<Stmt> &stmt, const PP &pp);

/*
 * Return the initial program point of a given stmt.
 */
[[nodiscard]] PP initial_program_point(const std::shared_ptr<Stmt> &stmt);

/*
 * Return the set of final program points of a given stmt.
 */
[[nodiscard]] std::set<PP> final_program_points(const std::shared_ptr<Stmt> &stmt);

/*
 * Checks whether the given program point is the initial program point in the given statement.
 */
[[nodiscard]] bool is_program_point_initial(const std::shared_ptr<Stmt> &stmt, const PP &pp);

/*
 * Checks whether the given program point is the initial program point in the given statement.
 */
[[nodiscard]] bool is_program_point_final(const std::shared_ptr<Stmt> &stmt, const PP &pp);

/*
 * Return the control flow of a given stmt.
 */
[[nodiscard]] std::set<std::pair<PP, PP>> control_flow(const std::shared_ptr<Stmt> &stmt);

/*
 * Checks whether a program has an isolated entry, i.e., there is no (pp, init(stmt)) in flow(stmt).
 */
[[nodiscard]] bool has_isolated_entries(const std::shared_ptr<Stmt> &stmt);

/*
 * Checks whether a program has isolated exits, i.e., there is no f_pp in final(stmt) with (f_pp, pp).
 */
[[nodiscard]] bool has_isolated_exits(const std::shared_ptr<Stmt> &stmt);
