#pragma once

#include <set>
#include <algorithm>

#include "ast.hpp"


// https://www.cppstories.com/2019/02/2lines3featuresoverload.html/
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;


//TODO: cpp datei

namespace dfa_utils
{

std::set<Var> free_variables_aexp(const std::unique_ptr<AExp>& aexp) {
    std::set<Var> free_vars{};

    auto visitor = overload {
      [&free_vars](const Var& var) { 
            free_vars.insert(var);
        },
      [&free_vars](const Num& num) {},
      [&free_vars](const ArithmeticOp& opa) {
            auto lhs_set = free_variables_aexp(opa.lhs_);
            auto rhs_set = free_variables_aexp(opa.rhs_);
            free_vars.insert(lhs_set.begin(), lhs_set.end());
            free_vars.insert(rhs_set.begin(), rhs_set.end());
        }
    };

    if (aexp) {
        std::visit(visitor, *aexp);
    }

    return free_vars; // should move using RVO?
}

std::set<Var> free_variables_bexp(const std::unique_ptr<BExp>& bexp) {
    std::set<Var> free_vars;

    auto visitor = overload {
        [&free_vars](const True& t) {},
        [&free_vars](const False& f) {},
        [&free_vars](const Not& n) {
            auto not_bexp_set = free_variables_bexp(n.b_);
            free_vars.insert(not_bexp_set.begin(), not_bexp_set.end());
        },
        [&free_vars](const BooleanOp& opb) {
            auto lhs_set = free_variables_bexp(opb.lhs_);
            auto rhs_set = free_variables_bexp(opb.rhs_);
            free_vars.insert(lhs_set.begin(), lhs_set.end());
            free_vars.insert(rhs_set.begin(), rhs_set.end());
        },
        [&free_vars](const RelationalOp& opr) {
            auto lhs_set = free_variables_aexp(opr.lhs_);
            auto rhs_set = free_variables_aexp(opr.rhs_);
            free_vars.insert(lhs_set.begin(), lhs_set.end());
            free_vars.insert(rhs_set.begin(), rhs_set.end());
        }
    };

    if (bexp) {
        std::visit(visitor, *bexp);
    }

    return free_vars;
}

std::set<Var> free_variables_stmt(const std::unique_ptr<Stmt>& stmt) {
    std::set<Var> free_vars{};

    auto visitor = overload {
        [&free_vars](const Skip& s) {},
        [&free_vars](const Assign& a) {
            auto aexp_set = free_variables_aexp(a.aexp_);
            free_vars.insert(*(a.var_));
            free_vars.insert(aexp_set.begin(), aexp_set.end());
        },
        [&free_vars](const If& i) {
            auto cond_set = free_variables_bexp(i.cond_->bexp_);
            auto then_set = free_variables_stmt(i.then_);
            auto else_set = free_variables_stmt(i.else_);
            free_vars.insert(cond_set.begin(), cond_set.end());
            free_vars.insert(then_set.begin(), then_set.end());
            free_vars.insert(else_set.begin(), else_set.end());
        },
        [&free_vars](const While& w) {
            auto cond_set = free_variables_bexp(w.cond_->bexp_);
            auto body_set = free_variables_stmt(w.body_);
            free_vars.insert(cond_set.begin(), cond_set.end());
            free_vars.insert(body_set.begin(), body_set.end());
        },
        [&free_vars](const SeqComp& sc) {
            auto fst_set = free_variables_stmt(sc.fst_);
            auto snd_set = free_variables_stmt(sc.snd_);
            free_vars.insert(fst_set.begin(), fst_set.end());
            free_vars.insert(snd_set.begin(), snd_set.end());
        }
    };

    if (stmt) {
        std::visit(visitor, *stmt);
    }

    return free_vars;
}


std::set<PP> program_points(const std::unique_ptr<Stmt>& stmt) {
    return {}; // TODO
}

std::set<Block> blocks(const std::unique_ptr<Stmt>& stmt) {
    std::set<Block> bs{};

    auto visitor = overload {
        [&bs](const Skip& s) {
            bs.insert(s);
        },
        [&bs](const Assign& a) {
            bs.insert(a);
        },
        [&bs](const If& i) {
            bs.insert(*(i.cond_));
            auto then_set = blocks(i.then_);
            auto else_set = blocks(i.else_);
            bs.insert(then_set.begin(), then_set.end());
            bs.insert(else_set.begin(), else_set.end());
        },
        [&bs](const While& w) {
            bs.insert(*(w.cond_));
            auto body_set = blocks(w.body_);
            bs.insert(body_set.begin(), body_set.end());
        },
        [&bs](const SeqComp& sc) {
            auto fst_set = blocks(sc.fst_);
            auto snd_set = blocks(sc.snd_);
            bs.insert(fst_set.begin(), fst_set.end());
            bs.insert(snd_set.begin(), snd_set.end());
        }
    };

    if (stmt) {
        std::visit(visitor, *stmt);
    }

    return bs;
}

}
