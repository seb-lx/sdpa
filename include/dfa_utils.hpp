#pragma once

#include <set>
#include <algorithm>
#include <utility>

#include "ast.hpp"
#include "ast_printer.hpp"


// https://www.cppstories.com/2019/02/2lines3featuresoverload.html/
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;


//TODO: cpp datei

namespace dfa_utils
{

using ControlFlowEdge = std::pair<PP, PP>;


/*
namespace {
    const Block* find_block_in_stmt(const std::unique_ptr<Stmt>& stmt, const PP& pp) {
        if (!stmt) return nullptr;

        std::set<PP> pps = program_points(stmt);
        if (pps.contains(pp)) {
            return get_block(stmt, pp);
        }
        
        return nullptr;
    }
}*/


// Custom comparator for Block pointer
// so that std::set orders the elements according to the underlying block instead of memory addresses
struct BlockPtrCmp {
    bool operator()(const Block* lhs, const Block* rhs) const {
        return lhs->pp_ < rhs->pp_;
    }
};
// ... same for Var pointer
struct VarPtrCmp {
    bool operator()(const Var* lhs, const Var* rhs) const {
        return lhs->name_ < rhs->name_;
    }
};


std::set<const Var*, VarPtrCmp> free_variables_aexp(const std::unique_ptr<AExp>& aexp) {
    if (!aexp) throw std::invalid_argument("Given AExp is empty!");

    std::set<const Var*, VarPtrCmp> free_vars{};

    auto visitor = overload {
      [&free_vars](const Var& var) { 
            free_vars.insert(&var);
        },
      [&free_vars](const Num& num) {},
      [&free_vars](const ArithmeticOp& opa) {
            auto lhs_set = free_variables_aexp(opa.lhs_);
            auto rhs_set = free_variables_aexp(opa.rhs_);
            free_vars.merge(lhs_set);
            free_vars.merge(rhs_set);
        }
    };

    std::visit(visitor, *aexp);

    return free_vars; // should move using RVO?
}

std::set<const Var*, VarPtrCmp> free_variables_bexp(const std::unique_ptr<BExp>& bexp) {
    if (!bexp) throw std::invalid_argument("Given BExp is empty!");

    std::set<const Var*, VarPtrCmp> free_vars;

    auto visitor = overload {
        [&free_vars](const True& t) {},
        [&free_vars](const False& f) {},
        [&free_vars](const Not& n) {
            auto not_bexp_set = free_variables_bexp(n.b_);
            free_vars.merge(not_bexp_set);
        },
        [&free_vars](const BooleanOp& opb) {
            auto lhs_set = free_variables_bexp(opb.lhs_);
            auto rhs_set = free_variables_bexp(opb.rhs_);
            free_vars.merge(lhs_set);
            free_vars.merge(rhs_set);
        },
        [&free_vars](const RelationalOp& opr) {
            auto lhs_set = free_variables_aexp(opr.lhs_);
            auto rhs_set = free_variables_aexp(opr.rhs_);
            free_vars.merge(lhs_set);
            free_vars.merge(rhs_set);
        }
    };
        
    std::visit(visitor, *bexp);

    return free_vars;
}

std::set<const Var*, VarPtrCmp> free_variables_stmt(const std::unique_ptr<Stmt>& stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    std::set<const Var*, VarPtrCmp> free_vars{};

    auto visitor = overload {
        [&free_vars](const Skip& s) {},
        [&free_vars](const Assign& a) {
            auto aexp_set = free_variables_aexp(a.aexp_);
            free_vars.insert(a.var_.get());
            free_vars.merge(aexp_set);
        },
        [&free_vars](const If& i) {
            auto cond_set = free_variables_bexp(i.cond_->bexp_);
            auto then_set = free_variables_stmt(i.then_);
            auto else_set = free_variables_stmt(i.else_);
            free_vars.merge(cond_set);
            free_vars.merge(then_set);
            free_vars.merge(else_set);
        },
        [&free_vars](const While& w) {
            auto cond_set = free_variables_bexp(w.cond_->bexp_);
            auto body_set = free_variables_stmt(w.body_);
            free_vars.merge(cond_set);
            free_vars.merge(body_set);
        },
        [&free_vars](const SeqComp& sc) {
            auto fst_set = free_variables_stmt(sc.fst_);
            auto snd_set = free_variables_stmt(sc.snd_);
            free_vars.merge(fst_set);
            free_vars.merge(snd_set);
        }
    };

    std::visit(visitor, *stmt);

    return free_vars;
}


std::set<PP> program_points(const std::unique_ptr<Stmt>& stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    std::set<PP> pps{};

    auto visitor = overload {
        [&pps](const Skip& s) {
            pps.insert(s.pp_);
        },
        [&pps](const Assign& a) {
            pps.insert(a.pp_);
        },
        [&pps](const If& i) {
            pps.insert(i.cond_->pp_);
            auto then_set = program_points(i.then_);
            auto else_set = program_points(i.else_);
            pps.merge(then_set);
            pps.merge(else_set);
        },
        [&pps](const While& w) {
            pps.insert(w.cond_->pp_);
            auto body_set = program_points(w.body_);
            pps.merge(body_set);
        },
        [&pps](const SeqComp& sc) {
            auto fst_set = program_points(sc.fst_);
            auto snd_set = program_points(sc.snd_);
            pps.merge(fst_set);
            pps.merge(snd_set);
        }
    };

    std::visit(visitor, *stmt);

    return pps;
}

/**
 * Returns the elementary block for the given program point.
 * If the program point is not found within the stmt, return nullptr
 */
const Block* get_block(const std::unique_ptr<Stmt>& stmt, const PP pp) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto visitor = overload {
        [&pp](const Skip& s) -> const Block* {
            return (s.pp_ == pp) ? &s : nullptr;
        },
        [&pp](const Assign& a) -> const Block* {
            return (a.pp_ == pp) ? &a : nullptr;
        },
        [&pp](const If& i) -> const Block* {
            if (i.cond_->pp_ == pp) {
                const Block* cond_ptr = i.cond_.get();
                return cond_ptr;
            }

            std::set<PP> pps_then = program_points(i.then_);
            if (pps_then.contains(pp)) {
                return get_block(i.then_, pp);
            }

            std::set<PP> pps_else = program_points(i.else_);
            if (pps_else.contains(pp)) {
                return get_block(i.else_, pp);
            }

            return nullptr;
        },
        [&pp](const While& w) -> const Block* {
            if (w.cond_->pp_ == pp) {
                const Block* cond_ptr = w.cond_.get();
                return cond_ptr;
            }

            std::set<PP> pps_body = program_points(w.body_);
            if (pps_body.contains(pp)) {
                return get_block(w.body_, pp);
            }

            return nullptr;
        },
        [&pp](const SeqComp& sc) -> const Block* {
            std::set<PP> pps_fst = program_points(sc.fst_);
            if (pps_fst.contains(pp)) {
                return get_block(sc.fst_, pp);
            }

            std::set<PP> pps_snd = program_points(sc.snd_);
            if (pps_snd.contains(pp)) {
                return get_block(sc.snd_, pp);
            }

            return nullptr;
        } 
    };
        
    return std::visit(visitor, *stmt);
} 

std::set<const Block*, BlockPtrCmp> blocks(const std::unique_ptr<Stmt>& stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    std::set<const Block*, BlockPtrCmp> bs{};

    auto visitor = overload {
        [&bs](const Skip& s) {
            bs.insert(&s);
        },
        [&bs](const Assign& a) {
            bs.insert(&a);
        },
        [&bs](const If& i) {
            bs.insert(i.cond_.get());
            auto then_set = blocks(i.then_);
            auto else_set = blocks(i.else_);
            bs.merge(then_set);
            bs.merge(else_set);
        },
        [&bs](const While& w) {
            bs.insert(w.cond_.get());
            auto body_set = blocks(w.body_);
            bs.merge(body_set);
        },
        [&bs](const SeqComp& sc) {
            auto fst_set = blocks(sc.fst_);
            auto snd_set = blocks(sc.snd_);
            bs.merge(fst_set);
            bs.merge(snd_set);
        }
    };

    std::visit(visitor, *stmt);

    return bs;
}

bool well_formed(const std::unique_ptr<Stmt>& stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto visitor = overload {
        [](const Skip& s) {
            return true;
        },
        [](const Assign& a) {
            return true;
        },
        [](const If& i) {
            bool well_formed_then = well_formed(i.then_);
            bool well_formed_else = well_formed(i.else_);
            
            auto pps_then = program_points(i.then_);
            auto pps_else = program_points(i.else_);

            bool then_else_intersect = set_utilities::intersect(pps_then, pps_else);

            bool pp_in_then = pps_then.contains(i.cond_->pp_);
            bool pp_in_else = pps_else.contains(i.cond_->pp_);

            return well_formed_then && well_formed_else && !then_else_intersect && !pp_in_then && !pp_in_else;
        },
        [](const While& w) {
            bool well_formed_body = well_formed(w.body_);
            
            auto pps_body = program_points(w.body_);

            bool pp_in_body = pps_body.contains(w.cond_->pp_);

            return well_formed_body && !pp_in_body;
        },
        [](const SeqComp& sc) {
            bool well_formed_fst = well_formed(sc.fst_);
            bool well_formed_snd = well_formed(sc.snd_);
            
            auto pps_fst = program_points(sc.fst_);
            auto pps_snd = program_points(sc.snd_);

            bool fst_snd_intersect = set_utilities::intersect(pps_fst, pps_fst);

            return well_formed_fst && well_formed_snd && !fst_snd_intersect;
        }
    };

    return std::visit(visitor, *stmt);
}

unsigned int pp_occurences(const std::unique_ptr<Stmt>& stmt, const PP pp) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto visitor = overload {
        [&pp](const Skip& s) {
            return (s.pp_ == pp) ? 1 : 0;
        },
        [&pp](const Assign& a) {
            return (a.pp_ == pp) ? 1 : 0;
        },
        [&pp](const If& i) {
            auto pp_occurences_then = pp_occurences(i.then_, pp);
            auto pp_occurences_else = pp_occurences(i.else_, pp);
            auto pp_occurences_then_else = pp_occurences_then + pp_occurences_else;
            
            return (i.cond_->pp_ == pp) ? (1 + pp_occurences_then_else) : pp_occurences_then_else;
        },
        [&pp](const While& w) {
            auto pp_occurences_body = pp_occurences(w.body_, pp);
            
            return (w.cond_->pp_ == pp) ? (1 + pp_occurences_body) : pp_occurences_body;
        },
        [&pp](const SeqComp& sc) {
            auto pp_occurences_fst = pp_occurences(sc.fst_, pp);
            auto pp_occurences_snd = pp_occurences(sc.snd_, pp);
            
            return pp_occurences_fst + pp_occurences_snd;
        }
    };

    return std::visit(visitor, *stmt);
}

PP initial_pp(const std::unique_ptr<Stmt>& stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto visitor = overload {
        [](const Skip& s) {
            return s.pp_;
        },
        [](const Assign& a) {
            return a.pp_;
        },
        [](const If& i) {
            return i.cond_->pp_;
        },
        [](const While& w) {
            return w.cond_->pp_;
        },
        [](const SeqComp& sc) {
            return initial_pp(sc.fst_);
        }
    };

    return std::visit(visitor, *stmt);
}

bool is_initial_pp(const std::unique_ptr<Stmt>& stmt, const PP pp) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    return pp == initial_pp(stmt);
}

std::set<PP> final_pps(const std::unique_ptr<Stmt>& stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    std::set<PP> pps{};

    auto visitor = overload {
        [&pps](const Skip& s) {
            pps.insert(s.pp_);
        },
        [&pps](const Assign& a) {
            pps.insert(a.pp_);
        },
        [&pps](const If& i) {
            auto final_pps_then = final_pps(i.then_);
            auto final_pps_else = final_pps(i.else_);

            pps.merge(final_pps_then);
            pps.merge(final_pps_else);
        },
        [&pps](const While& w) {
            pps.insert(w.cond_->pp_);
        },
        [&pps](const SeqComp& sc) {
            auto final_pps_snd = final_pps(sc.snd_);
            pps.merge(final_pps_snd);
        }
    };

    std::visit(visitor, *stmt);

    return pps;
}

bool is_final_pp(const std::unique_ptr<Stmt>& stmt, const PP pp) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto final_pps_stmt = final_pps(stmt);

    return final_pps_stmt.contains(pp);
}


std::set<ControlFlowEdge> control_flow(const std::unique_ptr<Stmt>& stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    std::set<ControlFlowEdge> cf{};

    auto visitor = overload {
        [&cf](const Skip& s) {},
        [&cf](const Assign& a) {},
        [&cf](const If& i) {
            auto control_flow_then = control_flow(i.then_);
            auto control_flow_else = control_flow(i.else_);

            auto initial_pp_then = initial_pp(i.then_);
            auto initial_pp_else = initial_pp(i.else_);

            cf.merge(control_flow_then);
            cf.merge(control_flow_else);
            cf.insert({i.cond_->pp_, initial_pp_then});
            cf.insert({i.cond_->pp_, initial_pp_else});
        },
        [&cf](const While& w) {
            auto control_flow_body = control_flow(w.body_);

            auto initial_pp_body = initial_pp(w.body_);
            auto final_pp_body = final_pps(w.body_);

            std::set<ControlFlowEdge> cross_pp_final_body = set_utilities::cross<PP>(w.cond_->pp_, final_pp_body);

            cf.merge(control_flow_body);
            cf.insert({w.cond_->pp_, initial_pp_body});
            cf.merge(cross_pp_final_body);
        },
        [&cf](const SeqComp& sc) {
            auto control_flow_fst = control_flow(sc.fst_);
            auto control_flow_snd = control_flow(sc.snd_);

            auto final_pp_fst = final_pps(sc.fst_);
            auto initial_pp_snd = initial_pp(sc.snd_);

            std::set<ControlFlowEdge> cross_final_fst_init_snd = set_utilities::cross<PP>(initial_pp_snd, final_pp_fst);

            cf.merge(control_flow_fst);
            cf.merge(control_flow_snd);
            cf.merge(cross_final_fst_init_snd);
        }
    };

    std::visit(visitor, *stmt);

    return cf;
}

bool has_isolated_entries(const std::unique_ptr<Stmt>& stmt)
{
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto cf = control_flow(stmt);
    auto initial_pp_stmt = initial_pp(stmt);

    for (const auto& pp: program_points(stmt)) {
        if (cf.contains({pp, initial_pp_stmt})) {
            return false;
        }
    }

    return true;
}

bool has_isolated_exits(const std::unique_ptr<Stmt> &stmt)
{
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto cf = control_flow(stmt);
    auto final_pps_stmt = final_pps(stmt);

    for (const auto& edge: cf) {
        if (final_pps_stmt.contains(edge.first)) {
            return false;
        }
    }

    return true;
}

namespace set_utilities {

    template<typename T>
    bool intersect(const std::set<T>& set1, const std::set<T>& set2) {
        auto contains_lambda = [&set2](const T& v) {
            return set2.contains(v);
        };

        return std::ranges::any_of(set1, contains_lambda);
    }

    template<typename T>
    auto cross(T x, const std::set<T>& s) {
        return s | std::views::transform([x](T num) {
            return std::make_pair(x, num);
        });
    }
}

namespace io {
    // Printer methods
    
    void print_var_set(const std::set<const Var*, VarPtrCmp>& vars) {
        std::cout << "Variables: { ";
        for (auto it = vars.cbegin(); it != vars.cend(); ++it) {
            std::cout << (*it)->name_;
            if (std::next(it) != vars.end()) {
                std::cout << ", ";
            }
        }
        std::cout << " }\n";
    }

    void print_pp_set(const std::set<PP>& pps) {
        std::cout << "Program points: { ";
        for (auto it = pps.cbegin(); it != pps.cend(); ++it) {
            std::cout << *it;
            if (std::next(it) != pps.end()) {
                std::cout << ", ";
            }
        }
        std::cout << " }\n";
    }

    void print_block_set(const std::set<const Block*, BlockPtrCmp>& blocks) {
        std::cout << "Blocks: { ";
        ASTSimplePrinter printer;
        for (auto it = blocks.cbegin(); it != blocks.cend(); ++it) {
            auto b = *it;

            if (auto skip_block = dynamic_cast<const Skip*>(b)) {
                printer.print(*skip_block);
            } else if (auto assign_block = dynamic_cast<const Assign*>(b)) {
                std::cout << "[";
                printer.print(*(assign_block->var_));
                std::cout << " := ";
                printer.print(*(assign_block->aexp_));
                std::cout << "]^";
                std::cout << assign_block->pp_;
               
            } else if (auto cond_block = dynamic_cast<const Cond*>(b)) {
                std::cout << "[";
                printer.print(*(cond_block->bexp_));
                std::cout << "]^";
                std::cout << cond_block->pp_;
            }

            if (std::next(it) != blocks.end()) {
                std::cout << ", ";
            }
        }
        std::cout << " }\n";
    }
}

}
