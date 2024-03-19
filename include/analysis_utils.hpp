#pragma once

#include <algorithm>
#include <iterator>
#include<set>

#include "ast.hpp"

[[nodiscard]] std::set<Var> free_variables_a_exp(const std::shared_ptr<AExp> &a_exp) noexcept;
[[nodiscard]] std::set<Var> free_variables_b_exp(const std::shared_ptr<BExp> &b_exp) noexcept;
[[nodiscard]] std::set<Var> free_variables_stmt(const std::shared_ptr<Stmt> &stmt) noexcept;

[[nodiscard]] std::set<PP> program_points(const std::shared_ptr<Stmt> &stmt) noexcept;
