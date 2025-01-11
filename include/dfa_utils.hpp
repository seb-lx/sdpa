#pragma once

#include <set>
#include <algorithm>

#include "ast.hpp"


// https://www.cppstories.com/2019/02/2lines3featuresoverload.html/
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;



namespace dfa_utilities
{

std::set<Var> free_variables_aexp(const std::unique_ptr<AExp>& aexp)
{
    std::set<Var> free_vars;

    auto visitor = overload {
      [&free_vars](const Var& var) { 
            free_vars.insert(var);
        },
      [&free_vars](const Num& num) {},
      [&free_vars](const ArithmeticOp& opa) {
            auto lhs_set = free_variables_aexp(opa._lhs);
            auto rhs_set = free_variables_aexp(opa._rhs);
            free_vars.insert(lhs_set.begin(), lhs_set.end());
            free_vars.insert(rhs_set.begin(), rhs_set.end());
        }
    };

    if (aexp) std::visit(visitor, *aexp);

    return free_vars; // should move using RVO?
}

std::set<Var> free_variables_bexp(const std::unique_ptr<BExp>& bexp)
{
    std::set<Var> free_vars;

    auto visitor = overload {
        [&free_vars](const True& t) {},
        [&free_vars](const False& f) {},
        [&free_vars](const Not& n) {
            auto not_bexp_set = free_variables_bexp(n._b);
            free_vars.insert(not_bexp_set.begin(), not_bexp_set.end());
        },
        [&free_vars](const BooleanOp& opb) {
            auto lhs_set = free_variables_bexp(opb._lhs);
            auto rhs_set = free_variables_bexp(opb._rhs);
            free_vars.insert(lhs_set.begin(), lhs_set.end());
            free_vars.insert(rhs_set.begin(), rhs_set.end());
        },
        [&free_vars](const RelationalOp& opr) {
            auto lhs_set = free_variables_aexp(opr._lhs);
            auto rhs_set = free_variables_aexp(opr._rhs);
            free_vars.insert(lhs_set.begin(), lhs_set.end());
            free_vars.insert(rhs_set.begin(), rhs_set.end());
        }
    };

    if (bexp) std::visit(visitor, *bexp);

    return free_vars; // should move using RVO?
}

std::set<Var> free_variables_stmt(const std::unique_ptr<Stmt>& stmt)
{
    std::set<Var> free_vars;

    auto visitor = overload {
        [&free_vars](const Skip& s) {},
        [&free_vars](const Assign& a) {
            auto aexp_set = free_variables_aexp(a._aexp);
            free_vars.insert(*(a._var));
            free_vars.insert(aexp_set.begin(), aexp_set.end());
        },
        [&free_vars](const If& i) {
            auto cond_set = free_variables_bexp(i._cond);
            auto then_set = free_variables_stmt(i._then);
            auto else_set = free_variables_stmt(i._else);
            free_vars.insert(cond_set.begin(), cond_set.end());
            free_vars.insert(then_set.begin(), then_set.end());
            free_vars.insert(else_set.begin(), else_set.end());
        },
        [&free_vars](const While& w) {
            auto cond_set = free_variables_bexp(w._cond);
            auto body_set = free_variables_stmt(w._body);
            free_vars.insert(cond_set.begin(), cond_set.end());
            free_vars.insert(body_set.begin(), body_set.end());
        },
        [&free_vars](const SeqComp& sc) {
            auto fst_set = free_variables_stmt(sc._fst);
            auto snd_set = free_variables_stmt(sc._snd);
            free_vars.insert(fst_set.begin(), fst_set.end());
            free_vars.insert(snd_set.begin(), snd_set.end());
        }
    };

    if (stmt) std::visit(visitor, *stmt);

    return free_vars; // should move using RVO?
}

}
