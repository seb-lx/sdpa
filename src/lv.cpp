#include "lv.hpp"
#include "dfa_utils.hpp"


auto LiveVariableAnalysis::compute() const -> LiveVariablesVec {
    // The vector holds #pp * 2 elements, for each pp entry and exit information
    const unsigned int vec_size = n_ * 2;
    LiveVariablesVec vec(vec_size);
    LiveVariablesVec prev_vec{vec};

    // Iterate until fixpoint reached
    unsigned int iteration = 1;
    while(true) {
        vec = F_LV(vec);

        if (fixpoint_reached(prev_vec, vec)) {
            break;
        }

        prev_vec = vec;
        ++iteration;
    }

    std::cout << "LV-analysis in " << iteration << " iterations.\n";
    return vec;
}

auto LiveVariableAnalysis::F_LV(const LiveVariablesVec& v) const -> LiveVariablesVec {
    LiveVariablesVec vec(v.size());

    for (auto i = 0; i < v.size() / 2; ++i) {
        const PP f_i = f(i);

        // vec[2*i]
        const auto pp_block = dfa_utils::get_block(stmt_, f_i);
        LiveVariables s = v[2*i + 1];
        LiveVariables to_kill = kill_LV(pp_block);
        LiveVariables to_gen = gen_LV(pp_block);
        LiveVariables s_without_to_kill = var_ptr_set_difference(s, to_kill);
        s_without_to_kill.merge(to_gen);
        vec[2*i] = s_without_to_kill;

        // vec[2*i + 1]
        if (final_pps_.contains(f_i))
            vec[2*i + 1] = {};
        else {
            LiveVariables union_entries_succ{};
            for (const auto& edge: cf_) {
                if (edge.first == f_i) {
                    const PP f_j = edge.second;
                    const PP j = f_j - 1;
                    LiveVariables lvs = v[2*j];
                    union_entries_succ.merge(lvs);
                }
            }
            vec[2*i + 1] = union_entries_succ;
        }
    }

    return vec;
}

PP LiveVariableAnalysis::f(unsigned int i) const {
    unsigned int pp_idx = i + 1;
    for (const auto& e: pps_) { if (e == pp_idx) return e; }

    throw std::runtime_error("Invalid mapping index!");
}

bool LiveVariableAnalysis::fixpoint_reached(const LiveVariablesVec& vec1, const LiveVariablesVec& vec2) {
    if (vec1.size() != vec2.size()) throw std::runtime_error("Vector changed size?!");

    // Need to check if the input vectors are equal
    for (auto i = 0; i < vec1.size(); ++i) {
        if (vec1[i].size() != vec2[i].size()) {
            return false;
        }

        if (!var_ptr_set_equality(vec1[i], vec2[i])) {
            return false;
        } 
    }

    return true;
}

auto LiveVariableAnalysis::gen_LV(const Block* block) const -> LiveVariables {
    // Block is no std::variant, cant use std::visit here
    if (auto a = dynamic_cast<const Assign*>(block)) {
        return dfa_utils::free_variables_aexp(a->aexp_.get());

    } else if (auto s = dynamic_cast<const Skip*>(block)) {
        return {};

    } else if (auto c = dynamic_cast<const Cond*>(block)) {
        return dfa_utils::free_variables_bexp(c->bexp_.get());

    } else {
        throw std::runtime_error("Unknown Block!");
    }
}

auto LiveVariableAnalysis::kill_LV(const Block* block) const -> LiveVariables {
    // Block is no std::variant, cant use std::visit here
    if (auto a = dynamic_cast<const Assign*>(block)) {
        return { a->var_.get() };

    } else if (auto s = dynamic_cast<const Skip*>(block)) {
        return {};

    } else if (auto c = dynamic_cast<const Cond*>(block)) {
        return {};

    } else {
        throw std::runtime_error("Unknown Block!");
    }
}

/*void LiveVariableAnalysis::check_and_enforce_analysis_constraints(const std::unique_ptr<Stmt>& stmt)
{
    if (!dfa_utils::has_isolated_exits(stmt)) {
        // Wrap statement into sequential composition where a skip statement is added
        const PP added_pp = n_ + 1;
        auto added_skip_stmt = std::make_unique<Stmt>( Skip{ added_pp } );

        stmt_ = std::make_unique<Stmt>(
            SeqComp{
                stmt, // todo: problem since unique ptr ...
                std::move(added_skip_stmt)
            }
        ).get();
    }
}*/

void LiveVariableAnalysis::init(const Stmt* stmt) {
    pps_ = dfa_utils::program_points(stmt);
    n_ = pps_.size();
    cf_ = dfa_utils::control_flow(stmt);
    final_pps_ = dfa_utils::final_pps(stmt);
}

void LiveVariableAnalysis::print_result(const LiveVariablesVec& res) {
    std::cout << "Result of LV-analysis:\n";
    for(auto i = 0; i < res.size(); ++i) {
        std::cout << "\tvec[" << i << "]: ";
        if (res[i].empty()) std::cout << "{ }";
        else {
            std::cout << "{  ";
            for(const auto &se: res[i]) {
                std::cout << se->name_ << "  ";
            }
            std::cout << "}";
        }
        std::cout << "\n";
    }
}