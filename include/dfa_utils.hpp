#pragma once

#include <set>
#include <algorithm>

#include "ast.hpp"
#include "ast_printer.hpp"


// https://www.cppstories.com/2019/02/2lines3featuresoverload.html/
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;


//TODO: cpp datei

namespace dfa_utils
{

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

    if (aexp) {
        std::visit(visitor, *aexp);
    }

    return free_vars; // should move using RVO?
}

std::set<const Var*, VarPtrCmp> free_variables_bexp(const std::unique_ptr<BExp>& bexp) {
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

    if (bexp) {
        std::visit(visitor, *bexp);
    }

    return free_vars;
}

std::set<const Var*, VarPtrCmp> free_variables_stmt(const std::unique_ptr<Stmt>& stmt) {
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

    if (stmt) {
        std::visit(visitor, *stmt);
    }

    return free_vars;
}


std::set<PP> program_points(const std::unique_ptr<Stmt>& stmt) {
    std::set<PP> pps{};

    if (!stmt) return pps;

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

 //TODO for LV: final pps, control flow, 


/**
 * Returns the elementary block for the given program point.
 * If the program point is not found within the stmt, return nullptr
 */
const Block* get_block(const std::unique_ptr<Stmt>& stmt, const PP pp) {
    if (!stmt) return nullptr;

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

    if (stmt) {
        std::visit(visitor, *stmt);
    }

    return bs;
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
