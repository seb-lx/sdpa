#pragma once

#include "utils.hpp"


std::set<PP> pp_set_intersect(const std::set<PP>& set1, const std::set<PP>& set2);

/**
 * Calculates set1 \ set2, i.e. all values in set1 that are not in set2
 */
auto var_ptr_set_difference(
    const std::set<const Var*, VarPtrCmp>& set1,
    const std::set<const Var*, VarPtrCmp>& set2
) -> std::set<const Var*, VarPtrCmp>;

bool var_ptr_set_equality(
    const std::set<const Var*, VarPtrCmp>& set1,
    const std::set<const Var*, VarPtrCmp>& set2
);
