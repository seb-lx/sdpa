#include "analysis_utils.hpp"

/*
 * Compute the set of free variables in the given AExp.
 */
std::set<Var> free_variables_a_exp(const std::shared_ptr<AExp> &a_exp) noexcept
{
    if (Var* var = dynamic_cast<Var*>(a_exp.get())) {
        return std::set<Var>{*var};
    } else if (Num* num = dynamic_cast<Num*>(a_exp.get())) {
        return {};
    } else if (OpA* op_A = dynamic_cast<OpA*>(a_exp.get())) {
        std::set<Var> lhs_set = free_variables_a_exp(op_A->lhs);
        std::set<Var> rhs_set = free_variables_a_exp(op_A->rhs);
        std::set<Var> res;
        std::set_union(
            lhs_set.begin(), lhs_set.end(),
            rhs_set.begin(), rhs_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    }
}
