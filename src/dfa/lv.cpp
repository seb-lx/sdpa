#include "dfa/lv.hpp"
#include "../test/test_analysis_utils.hpp"


std::vector<std::set<Var>> LiveVariableAnalysis::compute() const
{
    // The vector holds #pp * 2 elements, for each pp entry and exit information
    unsigned int vec_size = n * 2;
    std::vector<std::set<Var>> vec(vec_size);

    // Iterate until fixpoint reached
    while(true) {
        std::vector<std::set<Var>> prev_vec = vec;
        this->F_LV(vec);
        if (fixpoint_reached(prev_vec, vec)) break;
    }

    return vec;
}

void LiveVariableAnalysis::F_LV(std::vector<std::set<Var>> &v) const
{
    // TODO
}

bool LiveVariableAnalysis::fixpoint_reached(const std::vector<std::set<Var>> &vec_1,
                                            const std::vector<std::set<Var>> &vec_2)
{
    if (vec_1.size() != vec_2.size()) return false;

    for (std::vector<std::set<Var>>::size_type i = 0; i < vec_1.size(); ++i)
    {
        if (vec_1[i].size() != vec_2[i].size()) return false;

        if (!are_sets_equal<Var>(vec_1[i], vec_2[i])) return false;
    }

    return true;
}

std::set<Var> LiveVariableAnalysis::get_LV(const std::shared_ptr<Block> &block) const
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
