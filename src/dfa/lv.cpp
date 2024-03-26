#include "dfa/lv.hpp"
#include "../test/test_analysis_utils.hpp"
#include "set_utils.hpp"


std::vector<std::set<Var>> LiveVariableAnalysis::compute() const
{
    // The vector holds #pp * 2 elements, for each pp entry and exit information
    unsigned int vec_size = n * 2;
    std::vector<std::set<Var>> vec(vec_size);

    // Iterate until fixpoint reached
    while(true) {
        std::vector<std::set<Var>> prev_vec = vec;
        vec = this->F_LV(vec);
        if (fixpoint_reached(prev_vec, vec)) break;
    }

    return vec;
}

std::vector<std::set<Var>> LiveVariableAnalysis::F_LV(const std::vector<std::set<Var>> &v) const
{
    std::vector<std::set<Var>> vec(v.size());

    for (auto i = 0; i < n; ++i)
    {
        PP f_i = f(i);

        if (i % 2 == 0) // even
        {
            std::shared_ptr<Block> pp_block = get_elementary_block(stmt, f_i);
            const std::set<Var> &s = v[2*i + 1];
            std::set<Var> to_kill = kill_LV(pp_block);
            std::set<Var> to_gen = gen_LV(pp_block);
            std::set<Var> s_without_to_kill = SetOps::diff_sets(s, to_kill);

            vec[2*i] = SetOps::join_sets(s_without_to_kill, to_gen);

        }
        else // odd
        {
            if (SetOps::is_value_in_set(f_i, final_pps))
                vec[2*i + 1] = {};
            else {
                std::set<Var> union_entries_succ;
                for (const auto &edge: cf) {
                    if (edge.first == f_i) {
                        PP f_j = edge.second;
                        union_entries_succ = SetOps::join_sets(union_entries_succ, v[2*(f_j.pp)]);
                    }
                }
                vec[2*i + 1] = union_entries_succ;
            }
        }
    }

    return vec;
}

PP LiveVariableAnalysis::f(unsigned int i) const
{
    unsigned int pp_idx = i + 1;
    for (const auto &e: pps) { if (e.pp == pp_idx) return e; }

    throw std::runtime_error("Invalid mapping index!");
}

bool LiveVariableAnalysis::fixpoint_reached(const std::vector<std::set<Var>> &vec_1,
                                            const std::vector<std::set<Var>> &vec_2)
{
    if (vec_1.size() != vec_2.size()) return false;

    for (std::vector<std::set<Var>>::size_type i = 0; i < vec_1.size(); ++i)
    {
        if (vec_1[i].size() != vec_2[i].size()) return false;

        if (!SetOps::are_sets_equal<Var>(vec_1[i], vec_2[i])) return false;
    }

    return true;
}

std::set<Var> LiveVariableAnalysis::gen_LV(const std::shared_ptr<Block> &block) const
{
    if (std::shared_ptr<Ass> ass_block = std::dynamic_pointer_cast<Ass>(block)) {
        return free_variables_a_exp(ass_block->a_exp);
    } else if (std::shared_ptr<Skip> skip_block = std::dynamic_pointer_cast<Skip>(block)) {
        return {};
    } else if (std::shared_ptr<Cond> cond_block = std::dynamic_pointer_cast<Cond>(block)) {
        return free_variables_b_exp(cond_block->b_exp);
    } else {
        throw std::runtime_error("Unknown Block!");
    }
}

std::set<Var> LiveVariableAnalysis::kill_LV(const std::shared_ptr<Block> &block) const
{
    if (std::shared_ptr<Ass> ass_block = std::dynamic_pointer_cast<Ass>(block)) {
        return { *(ass_block->var) };
    } else if (std::shared_ptr<Skip> skip_block = std::dynamic_pointer_cast<Skip>(block)) {
        return {};
    } else if (std::shared_ptr<Cond> cond_block = std::dynamic_pointer_cast<Cond>(block)) {
        return {};
    } else {
        throw std::runtime_error("Unknown Block!");
    }
}

void LiveVariableAnalysis::check_analysis_constraints()
{
    if (!has_isolated_exits(stmt)) {
        // TODO: add skip statement, cout warning?
    }
}
