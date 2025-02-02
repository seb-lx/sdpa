#include "set_utils.hpp"

std::set<PP> pp_set_intersect(const std::set<PP>& set1, const std::set<PP>& set2) {
    std::set<PP> res;

    std::set_intersection(
        set1.begin(), set1.end(),
        set2.begin(), set2.end(),
        std::inserter(res, res.begin())
    );

    return res;
}   

/**
 * Calculates set1 \ set2, i.e. all values in set1 that are not in set2
 */
auto var_ptr_set_difference(
    const std::set<const Var*, VarPtrCmp>& set1,
    const std::set<const Var*, VarPtrCmp>& set2
) -> std::set<const Var*, VarPtrCmp>
{
    std::set<const Var*, VarPtrCmp> res;

    std::unordered_set<std::string> set2_names{}; // better lookup performance?
    for (const auto v: set2) set2_names.insert(v->name_);

    for (const Var* v: set1) {
        if (set2_names.find(v->name_) == set2_names.end()) {
            res.insert(v); // insert value in set1 if its not in set2
        }
    }

    return res;
}

bool var_ptr_set_equality(
    const std::set<const Var*, VarPtrCmp>& set1,
    const std::set<const Var*, VarPtrCmp>& set2
) {
    if (set1.size() != set2.size()) {
        return false;
    }

    auto it1 = set1.begin();
    auto it2 = set2.begin();

    while (it1 != set1.end() && it2 != set2.end()) {
        if ((*it1)->name_ != (*it2)->name_) {
            return false;
        }

        ++it1; ++it2;
    }

    return true;
}