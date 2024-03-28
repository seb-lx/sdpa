#include "dfa/lv.hpp"
#include "../test/test_analysis_utils.hpp"
#include "set_utils.hpp"


std::vector<std::set<Var>> LiveVariableAnalysis::compute() const
{
    // The vector holds #pp * 2 elements, for each pp entry and exit information
    unsigned int vec_size = n * 2;
    std::vector<std::set<Var>> vec(vec_size);
    std::vector<std::set<Var>> prev_vec = vec;

    // Iterate until fixpoint reached
    unsigned int iteration = 1;
    while(true) {
        vec = this->F_LV(vec);
        if (fixpoint_reached(prev_vec, vec)) break;
        prev_vec = vec;
        ++iteration;
    }

    std::cout << "LV-analysis in " << iteration << " iterations.\n";
    return vec;
}

std::vector<std::set<Var>> LiveVariableAnalysis::F_LV(const std::vector<std::set<Var>> &v) const
{
    std::vector<std::set<Var>> vec(v.size());

    for (auto i = 0; i < v.size() / 2; ++i)
    {
        PP f_i = f(i);

        // vec[2*i]
        std::shared_ptr<Block> pp_block = get_elementary_block(stmt, f_i);
        const std::set<Var> &s = v[2*i + 1];
        std::set<Var> to_kill = kill_LV(pp_block);
        std::set<Var> to_gen = gen_LV(pp_block);
        std::set<Var> s_without_to_kill = SetOps::diff_sets(s, to_kill);
        vec[2*i] = SetOps::join_sets(s_without_to_kill, to_gen);

        // vec[2*i + 1]
        if (SetOps::is_value_in_set(f_i, final_pps))
            vec[2*i + 1] = {};
        else {
            std::set<Var> union_entries_succ;
            for (const auto &edge: cf) {
                if (edge.first == f_i) {
                    PP f_j = edge.second;
                    unsigned int j = f_j.pp - 1;
                    union_entries_succ = SetOps::join_sets(union_entries_succ, v[2*j]);
                }
            }
            vec[2*i + 1] = union_entries_succ;
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
    if (vec_1.size() != vec_2.size()) throw std::runtime_error("Vector changed size?!");

    for (auto i = 0; i < vec_1.size(); ++i)
    {
        if (vec_1[i].size() != vec_2[i].size()) return false;

        if (!SetOps::are_sets_equal<Var>(vec_1[i], vec_2[i])) return false;
    }

    return true;
}

std::set<Var> LiveVariableAnalysis::gen_LV(const std::shared_ptr<Block> &block) const
{
    if (auto ass_block = std::dynamic_pointer_cast<Ass>(block)) {
        return free_variables_a_exp(ass_block->a_exp);

    } else if (auto skip_block = std::dynamic_pointer_cast<Skip>(block)) {
        return {};

    } else if (auto cond_block = std::dynamic_pointer_cast<Cond>(block)) {
        return free_variables_b_exp(cond_block->b_exp);

    } else {
        throw std::runtime_error("Unknown Block!");
    }
}

std::set<Var> LiveVariableAnalysis::kill_LV(const std::shared_ptr<Block> &block) const
{
    if (auto ass_block = std::dynamic_pointer_cast<Ass>(block)) {
        return { *(ass_block->var) };

    } else if (auto skip_block = std::dynamic_pointer_cast<Skip>(block)) {
        return {};

    } else if (auto cond_block = std::dynamic_pointer_cast<Cond>(block)) {
        return {};

    } else {
        throw std::runtime_error("Unknown Block!");
    }
}

void LiveVariableAnalysis::check_and_enforce_analysis_constraints()
{
    if (!has_isolated_exits(stmt)) {
        // Wrap statement into sequential composition where a skip statement is added
        std::shared_ptr<PP> skip_pp = std::make_shared<PP>(n + 1);

        stmt = std::make_shared<SeqComp>(stmt, std::make_shared<Skip>(skip_pp));
    }
}

void LiveVariableAnalysis::init()
{
    pps = program_points(stmt);
    n = pps.size();
    cf = control_flow(stmt);
    final_pps = final_program_points(stmt);
}

void LiveVariableAnalysis::print_result(const std::vector<std::set<Var>> &res)
{
    std::cout << "\tResult of LV-analysis:\n";
    for(auto i = 0; i < res.size(); ++i) {
        std::cout << "\t\tvec[" << i << "]: ";
        if (res[i].empty()) std::cout << "{ }";
        else {
            std::cout << "{  ";
            for(const auto &s_e: res[i]) {
                std::cout << s_e.get_var() << "  ";
            }
            std::cout << "}";
        }
        std::cout << "\n";
    }
}
