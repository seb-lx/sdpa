#include "dfa_utils.hpp"


// dfa_utils

FreeVariables dfa_utils::free_variables_aexp(const AExp* aexp) {
    if (!aexp) throw std::invalid_argument("Given AExp is empty!");

    FreeVariables free_vars{};

    auto visitor = overload {
      [&free_vars](const Var& var) { 
            free_vars.insert(&var);
        },
      [&free_vars](const Num& num) {},
      [&free_vars](const ArithmeticOp& opa) {
            auto lhs_set = free_variables_aexp(opa.lhs_.get());
            auto rhs_set = free_variables_aexp(opa.rhs_.get());
            free_vars.merge(lhs_set);
            free_vars.merge(rhs_set);
        }
    };

    std::visit(visitor, *aexp);

    return free_vars; // should move using RVO?
}

FreeVariables dfa_utils::free_variables_bexp(const BExp* bexp) {
    if (!bexp) throw std::invalid_argument("Given BExp is empty!");

    FreeVariables free_vars;

    auto visitor = overload {
        [&free_vars](const True& t) {},
        [&free_vars](const False& f) {},
        [&free_vars](const Not& n) {
            auto not_bexp_set = free_variables_bexp(n.b_.get());
            free_vars.merge(not_bexp_set);
        },
        [&free_vars](const BooleanOp& opb) {
            auto lhs_set = free_variables_bexp(opb.lhs_.get());
            auto rhs_set = free_variables_bexp(opb.rhs_.get());
            free_vars.merge(lhs_set);
            free_vars.merge(rhs_set);
        },
        [&free_vars](const RelationalOp& opr) {
            auto lhs_set = free_variables_aexp(opr.lhs_.get());
            auto rhs_set = free_variables_aexp(opr.rhs_.get());
            free_vars.merge(lhs_set);
            free_vars.merge(rhs_set);
        }
    };
        
    std::visit(visitor, *bexp);

    return free_vars;
}

FreeVariables dfa_utils::free_variables_stmt(const Stmt* stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    FreeVariables free_vars{};

    auto visitor = overload {
        [&free_vars](const Skip& s) {},
        [&free_vars](const Assign& a) {
            auto aexp_set = free_variables_aexp(a.aexp_.get());
            free_vars.insert(a.var_.get());
            free_vars.merge(aexp_set);
        },
        [&free_vars](const If& i) {
            auto cond_set = free_variables_bexp(i.cond_->bexp_.get());
            auto then_set = free_variables_stmt(i.then_.get());
            auto else_set = free_variables_stmt(i.else_.get());
            free_vars.merge(cond_set);
            free_vars.merge(then_set);
            free_vars.merge(else_set);
        },
        [&free_vars](const While& w) {
            auto cond_set = free_variables_bexp(w.cond_->bexp_.get());
            auto body_set = free_variables_stmt(w.body_.get());
            free_vars.merge(cond_set);
            free_vars.merge(body_set);
        },
        [&free_vars](const SeqComp& sc) {
            auto fst_set = free_variables_stmt(sc.fst_.get());
            auto snd_set = free_variables_stmt(sc.snd_.get());
            free_vars.merge(fst_set);
            free_vars.merge(snd_set);
        }
    };

    std::visit(visitor, *stmt);

    return free_vars;
}

std::set<PP> dfa_utils::program_points(const Stmt* stmt) {
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
            auto then_set = program_points(i.then_.get());
            auto else_set = program_points(i.else_.get());
            pps.merge(then_set);
            pps.merge(else_set);
        },
        [&pps](const While& w) {
            pps.insert(w.cond_->pp_);
            auto body_set = program_points(w.body_.get());
            pps.merge(body_set);
        },
        [&pps](const SeqComp& sc) {
            auto fst_set = program_points(sc.fst_.get());
            auto snd_set = program_points(sc.snd_.get());
            pps.merge(fst_set);
            pps.merge(snd_set);
        }
    };

    std::visit(visitor, *stmt);

    return pps;
}


const Block* dfa_utils::get_block(const Stmt* stmt, const PP pp) {
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

            std::set<PP> pps_then = program_points(i.then_.get());
            if (pps_then.contains(pp)) {
                return get_block(i.then_.get(), pp);
            }

            std::set<PP> pps_else = program_points(i.else_.get());
            if (pps_else.contains(pp)) {
                return get_block(i.else_.get(), pp);
            }

            return nullptr;
        },
        [&pp](const While& w) -> const Block* {
            if (w.cond_->pp_ == pp) {
                const Block* cond_ptr = w.cond_.get();
                return cond_ptr;
            }

            std::set<PP> pps_body = program_points(w.body_.get());
            if (pps_body.contains(pp)) {
                return get_block(w.body_.get(), pp);
            }

            return nullptr;
        },
        [&pp](const SeqComp& sc) -> const Block* {
            std::set<PP> pps_fst = program_points(sc.fst_.get());
            if (pps_fst.contains(pp)) {
                return get_block(sc.fst_.get(), pp);
            }

            std::set<PP> pps_snd = program_points(sc.snd_.get());
            if (pps_snd.contains(pp)) {
                return get_block(sc.snd_.get(), pp);
            }

            return nullptr;
        } 
    };
        
    return std::visit(visitor, *stmt);
} 

ElementaryBlocks dfa_utils::blocks(const Stmt* stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    ElementaryBlocks bs{};

    auto visitor = overload {
        [&bs](const Skip& s) {
            bs.insert(&s);
        },
        [&bs](const Assign& a) {
            bs.insert(&a);
        },
        [&bs](const If& i) {
            bs.insert(i.cond_.get());
            auto then_set = blocks(i.then_.get());
            auto else_set = blocks(i.else_.get());
            bs.merge(then_set);
            bs.merge(else_set);
        },
        [&bs](const While& w) {
            bs.insert(w.cond_.get());
            auto body_set = blocks(w.body_.get());
            bs.merge(body_set);
        },
        [&bs](const SeqComp& sc) {
            auto fst_set = blocks(sc.fst_.get());
            auto snd_set = blocks(sc.snd_.get());
            bs.merge(fst_set);
            bs.merge(snd_set);
        }
    };

    std::visit(visitor, *stmt);

    return bs;
}

bool dfa_utils::well_formed(const Stmt* stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto visitor = overload {
        [](const Skip& s) {
            return true;
        },
        [](const Assign& a) {
            return true;
        },
        [](const If& i) {
            bool well_formed_then = well_formed(i.then_.get());
            bool well_formed_else = well_formed(i.else_.get());
            
            auto pps_then = program_points(i.then_.get());
            auto pps_else = program_points(i.else_.get());

            auto then_else_intersect = pp_set_intersect(pps_then, pps_else);

            bool pp_in_then = pps_then.contains(i.cond_->pp_);
            bool pp_in_else = pps_else.contains(i.cond_->pp_);

            auto res = well_formed_then && well_formed_else && then_else_intersect.empty() && !pp_in_then && !pp_in_else;

            return res;
        },
        [](const While& w) {
            bool well_formed_body = well_formed(w.body_.get());
            
            auto pps_body = program_points(w.body_.get());

            bool pp_in_body = pps_body.contains(w.cond_->pp_);

            auto res = well_formed_body && !pp_in_body;

            return res;
        },
        [](const SeqComp& sc) {
            bool well_formed_fst = well_formed(sc.fst_.get());
            bool well_formed_snd = well_formed(sc.snd_.get());
            
            auto pps_fst = program_points(sc.fst_.get());
            auto pps_snd = program_points(sc.snd_.get());

            auto fst_snd_intersect = pp_set_intersect(pps_fst, pps_snd);

            auto res = well_formed_fst && well_formed_snd && fst_snd_intersect.empty();

            return res;
        }
    };

    return std::visit(visitor, *stmt);
}

unsigned int dfa_utils::pp_occurences(const Stmt* stmt, const PP pp) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto visitor = overload {
        [&pp](const Skip& s) {
            return (s.pp_ == pp) ? static_cast<unsigned int>(1) : static_cast<unsigned int>(0);
        },
        [&pp](const Assign& a) {
            return (a.pp_ == pp) ? static_cast<unsigned int>(1) : static_cast<unsigned int>(0);
        },
        [&pp](const If& i) {
            auto pp_occurences_then = pp_occurences(i.then_.get(), pp);
            auto pp_occurences_else = pp_occurences(i.else_.get(), pp);
            auto pp_occurences_then_else = pp_occurences_then + pp_occurences_else;
            
            return (i.cond_->pp_ == pp) ? (1 + pp_occurences_then_else) : pp_occurences_then_else;
        },
        [&pp](const While& w) {
            auto pp_occurences_body = pp_occurences(w.body_.get(), pp);
            
            return (w.cond_->pp_ == pp) ? (1 + pp_occurences_body) : pp_occurences_body;
        },
        [&pp](const SeqComp& sc) {
            auto pp_occurences_fst = pp_occurences(sc.fst_.get(), pp);
            auto pp_occurences_snd = pp_occurences(sc.snd_.get(), pp);
            
            return pp_occurences_fst + pp_occurences_snd;
        }
    };

    return std::visit(visitor, *stmt);
}

PP dfa_utils::initial_pp(const Stmt* stmt) {
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
            return initial_pp(sc.fst_.get());
        }
    };

    return std::visit(visitor, *stmt);
}

bool dfa_utils::is_initial_pp(const Stmt* stmt, const PP pp) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    return pp == initial_pp(stmt);
}

std::set<PP> dfa_utils::final_pps(const Stmt* stmt) {
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
            auto final_pps_then = final_pps(i.then_.get());
            auto final_pps_else = final_pps(i.else_.get());

            pps.merge(final_pps_then);
            pps.merge(final_pps_else);
        },
        [&pps](const While& w) {
            pps.insert(w.cond_->pp_);
        },
        [&pps](const SeqComp& sc) {
            auto final_pps_snd = final_pps(sc.snd_.get());
            pps.merge(final_pps_snd);
        }
    };

    std::visit(visitor, *stmt);

    return pps;
}

bool dfa_utils::is_final_pp(const Stmt* stmt, const PP pp) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    auto final_pps_stmt = final_pps(stmt);

    return final_pps_stmt.contains(pp);
}


CFG dfa_utils::control_flow(const Stmt* stmt) {
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    CFG cf{};

    auto visitor = overload {
        [&cf](const Skip& s) {},
        [&cf](const Assign& a) {},
        [&cf](const If& i) {
            auto control_flow_then = control_flow(i.then_.get());
            auto control_flow_else = control_flow(i.else_.get());

            auto initial_pp_then = initial_pp(i.then_.get());
            auto initial_pp_else = initial_pp(i.else_.get());

            cf.merge(control_flow_then);
            cf.merge(control_flow_else);
            cf.insert({i.cond_->pp_, initial_pp_then});
            cf.insert({i.cond_->pp_, initial_pp_else});
        },
        [&cf](const While& w) {
            auto control_flow_body = control_flow(w.body_.get());

            auto initial_pp_body = initial_pp(w.body_.get());
            auto final_pp_body = final_pps(w.body_.get());

            
            CFG cross_pp_final_body{};
            for(const auto& final_pp: final_pp_body) {
                cross_pp_final_body.insert(std::make_pair(final_pp, w.cond_->pp_));
            }

            cf.merge(control_flow_body);
            cf.insert({w.cond_->pp_, initial_pp_body});
            cf.merge(cross_pp_final_body);
        },
        [&cf](const SeqComp& sc) {
            auto control_flow_fst = control_flow(sc.fst_.get());
            auto control_flow_snd = control_flow(sc.snd_.get());

            auto final_pp_fst = final_pps(sc.fst_.get());
            auto initial_pp_snd = initial_pp(sc.snd_.get());

            CFG cross_final_fst_init_snd{};
            for(const auto& final_pp: final_pp_fst) {
                cross_final_fst_init_snd.insert(std::make_pair(final_pp, initial_pp_snd));
            }

            cf.merge(control_flow_fst);
            cf.merge(control_flow_snd);
            cf.merge(cross_final_fst_init_snd);
        }
    };

    std::visit(visitor, *stmt);

    return cf;
}

bool dfa_utils::has_isolated_entries(const Stmt* stmt)
{
    if (!stmt) throw std::invalid_argument("Given Stmt is empty!");

    const auto cf = control_flow(stmt);
    const auto initial_pp_stmt = initial_pp(stmt);

    for (const auto& pp: program_points(stmt)) {
        const ControlFlowEdge edge = std::make_pair(pp, initial_pp_stmt);
        if (cf.contains(edge)) {
            return false;
        }
    }

    return true;
}

bool dfa_utils::has_isolated_exits(const Stmt* stmt)
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


// dfa_utils::io

void dfa_utils::io::print_var_set(const FreeVariables& vars) {
    std::cout << "Variables: { ";
    for (auto it = vars.cbegin(); it != vars.cend(); ++it) {
        std::cout << (*it)->name_;
        if (std::next(it) != vars.end()) {
            std::cout << ", ";
        }
    }
    std::cout << " }\n";
}

void dfa_utils::io::print_pp_set(const std::set<PP>& pps) {
    std::cout << "Program points: { ";
    for (auto it = pps.cbegin(); it != pps.cend(); ++it) {
        std::cout << *it;
        if (std::next(it) != pps.end()) {
            std::cout << ", ";
        }
    }
    std::cout << " }\n";
}

void dfa_utils::io::print_block_set(const ElementaryBlocks& blocks) {
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

void dfa_utils::io::print_cf_set(const CFG& cf) {
    std::cout << "Flow: { ";

    for (auto it = cf.cbegin(); it != cf.cend(); ++it) {
        auto edge = *it;

        std::cout << "(" << edge.first << ", " << edge.second << ")";

        if (std::next(it) != cf.end()) {
            std::cout << ", ";
        }
    }
    std::cout << " }\n";
}