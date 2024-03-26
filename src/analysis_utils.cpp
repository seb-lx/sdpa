#include "analysis_utils.hpp"


std::set<Var> free_variables_a_exp(const std::shared_ptr<AExp> &a_exp)
{
    // Todo: please give pattern matching ... seems not very elegant this way?!
    if (auto var = std::dynamic_pointer_cast<Var>(a_exp)) {
        return {*var};

    } else if (auto num = std::dynamic_pointer_cast<Num>(a_exp)) {
        return {};

    } else if (auto op_A = std::dynamic_pointer_cast<OpA>(a_exp)) {
        std::set<Var> lhs_set = free_variables_a_exp(op_A->lhs);
        std::set<Var> rhs_set = free_variables_a_exp(op_A->rhs);
        return SetOps::join_sets(lhs_set, rhs_set);

    } else {
        throw std::runtime_error("Unknown AExp!");
    }
}

std::set<Var> free_variables_b_exp(const std::shared_ptr<BExp> &b_exp)
{
    if (auto b = std::dynamic_pointer_cast<Bool>(b_exp)) {
        return {};

    } else if (auto n = std::dynamic_pointer_cast<Not>(b_exp)) {
        return free_variables_b_exp(n->b_exp);

    } else if (auto op_R = std::dynamic_pointer_cast<OpR>(b_exp)) {
        std::set<Var> lhs_set = free_variables_a_exp(op_R->lhs);
        std::set<Var> rhs_set = free_variables_a_exp(op_R->rhs);
        return SetOps::join_sets(lhs_set, rhs_set);

    } else if (auto op_B = std::dynamic_pointer_cast<OpB>(b_exp)) {
        std::set<Var> lhs_set = free_variables_b_exp(op_B->lhs);
        std::set<Var> rhs_set = free_variables_b_exp(op_B->rhs);
        return SetOps::join_sets(lhs_set, rhs_set);

    } else {
        throw std::runtime_error("Unknown BExp!");
    }
}

std::set<Var> free_variables_stmt(const std::shared_ptr<Stmt> &stmt)
{
    if (auto ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        std::set<Var> fst_set = {*(ass_stmt->var)};
        std::set<Var> snd_set = free_variables_a_exp(ass_stmt->a_exp);
        return SetOps::join_sets(fst_set, snd_set);

    } else if (auto skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return {};

    } else if (auto if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        std::set<Var> cond_set = free_variables_b_exp(if_stmt->cond->b_exp);
        std::set<Var> if_branch_set = free_variables_stmt(if_stmt->if_branch);
        std::set<Var> else_branch_set = free_variables_stmt(if_stmt->else_branch);
        return SetOps::join_sets(cond_set, SetOps::join_sets(if_branch_set, else_branch_set));

    } else if (auto while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        std::set<Var> body_set = free_variables_stmt(while_stmt->body);
        std::set<Var> cond_set = free_variables_b_exp(while_stmt->cond->b_exp);
        return SetOps::join_sets(body_set, cond_set);

    } else if (auto seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        std::set<Var> stmt_1_set = free_variables_stmt(seq_comp_stmt->stmt_1);
        std::set<Var> stmt_2_set = free_variables_stmt(seq_comp_stmt->stmt_2);
        return SetOps::join_sets(stmt_1_set, stmt_2_set);

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

std::set<PP> program_points(const std::shared_ptr<Stmt> &stmt)
{
    if (auto ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return {*(ass_stmt->get_program_point())};

    } else if (auto skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return {*(skip_stmt->get_program_point())};

    } else if (auto if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        std::set<PP> cond_set = { *(if_stmt->cond->get_program_point()) };
        std::set<PP> if_branch_set = program_points(if_stmt->if_branch);
        std::set<PP> else_branch_set = program_points(if_stmt->else_branch);
        return SetOps::join_sets(cond_set, SetOps::join_sets(if_branch_set, else_branch_set));

    } else if (auto while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        std::set<PP> cond_set = { *(while_stmt->cond->get_program_point()) };
        std::set<PP> body_set = program_points(while_stmt->body);
        return SetOps::join_sets(cond_set, body_set);

    } else if (auto seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        std::set<PP> stmt_1_set = program_points(seq_comp_stmt->stmt_1);
        std::set<PP> stmt_2_set = program_points(seq_comp_stmt->stmt_2);
        return SetOps::join_sets(stmt_1_set, stmt_2_set);

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

bool is_program_well_formed(const std::shared_ptr<Stmt> &stmt)
{
    if (auto ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return true;

    } else if (auto skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return true;

    } else if (auto if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        bool wf_if_branch = is_program_well_formed(if_stmt->if_branch);
        bool wf_else_branch = is_program_well_formed(if_stmt->else_branch);

        std::set<PP> pp_if_branch = program_points(if_stmt->if_branch);
        std::set<PP> pp_else_branch = program_points(if_stmt->else_branch);

        std::set<PP> branch_intersection = SetOps::intersect_sets(pp_if_branch, pp_else_branch);
        std::set<PP> branch_union = SetOps::join_sets(pp_if_branch, pp_else_branch);

        bool pp_not_in_branches = SetOps::is_value_not_in_set(*(if_stmt->cond->get_program_point()), branch_union);

        return wf_if_branch && wf_else_branch && branch_intersection.empty() && pp_not_in_branches;

    } else if (auto while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        bool wf_body = is_program_well_formed(while_stmt->body);
        std::set<PP> pp_body = program_points(while_stmt->body);
        bool pp_not_in_body = SetOps::is_value_not_in_set(*(while_stmt->cond->get_program_point()), pp_body);
        return wf_body && pp_not_in_body;

    } else if (auto seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        std::set<PP> pp_stmt_1 = program_points(seq_comp_stmt->stmt_1);
        std::set<PP> pp_stmt_2 = program_points(seq_comp_stmt->stmt_2);

        bool wf_stmt_1 = is_program_well_formed(seq_comp_stmt->stmt_1);
        bool wf_stmt_2 = is_program_well_formed(seq_comp_stmt->stmt_2);

        std::set<PP> stmt_intersection = SetOps::intersect_sets(pp_stmt_1, pp_stmt_2);

        return wf_stmt_1 && wf_stmt_2 && stmt_intersection.empty();

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

unsigned int program_point_occurrences(const std::shared_ptr<Stmt> &stmt, const PP &pp)
{
    if (auto ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return (*(ass_stmt->get_program_point()) == pp) ? 1 : 0;

    } else if (auto skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return (*(skip_stmt->get_program_point()) == pp) ? 1 : 0;

    } else if (auto if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        unsigned int pp_occurrences_if_branch = program_point_occurrences(if_stmt->if_branch, pp);
        unsigned int pp_occurrences_else_branch = program_point_occurrences(if_stmt->else_branch, pp);

        if (*(if_stmt->cond->get_program_point()) == pp)
            return pp_occurrences_if_branch + pp_occurrences_else_branch + 1;
        else
            return pp_occurrences_if_branch + pp_occurrences_else_branch;

    } else if (auto while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        unsigned int pp_occurrences_body = program_point_occurrences(while_stmt->body, pp);

        if (*(while_stmt->cond->get_program_point()) == pp)
            return pp_occurrences_body + 1;
        else
            return pp_occurrences_body;

    } else if (auto seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        unsigned int pp_occurrences_stmt_1 = program_point_occurrences(seq_comp_stmt->stmt_1, pp);
        unsigned int pp_occurrences_stmt_2 = program_point_occurrences(seq_comp_stmt->stmt_2, pp);

        return pp_occurrences_stmt_1 + pp_occurrences_stmt_2;

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

std::set<std::shared_ptr<Block>> elementary_blocks(const std::shared_ptr<Stmt> &stmt)
{
    if (auto ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return {ass_stmt};

    } else if (auto skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return {skip_stmt};

    } else if (auto if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        std::set<std::shared_ptr<Block>> cond_set = {if_stmt->cond};
        std::set<std::shared_ptr<Block>> if_branch_set = elementary_blocks(if_stmt->if_branch);
        std::set<std::shared_ptr<Block>> else_branch_set = elementary_blocks(if_stmt->else_branch);

        return SetOps::join_sets(cond_set, SetOps::join_sets(if_branch_set, else_branch_set));

    } else if (auto while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        std::set<std::shared_ptr<Block>> cond_set = {while_stmt->cond};
        std::set<std::shared_ptr<Block>> body_set = elementary_blocks(while_stmt->body);

        return SetOps::join_sets(cond_set, body_set);

    } else if (auto seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        std::set<std::shared_ptr<Block>> stmt_1_set = elementary_blocks(seq_comp_stmt->stmt_1);
        std::set<std::shared_ptr<Block>> stmt_2_set = elementary_blocks(seq_comp_stmt->stmt_2);

        return SetOps::join_sets(stmt_1_set, stmt_2_set);

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

std::shared_ptr<Block> get_elementary_block(const std::shared_ptr<Stmt> &stmt, const PP &pp)
{
    if (auto ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        if (*(ass_stmt->get_program_point()) == pp) return ass_stmt;

    } else if (auto skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        if (*(skip_stmt->get_program_point()) == pp) return skip_stmt;

    } else if (auto if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        if (*(if_stmt->cond->get_program_point()) == pp)
            return if_stmt->cond;

        else if (*(if_stmt->cond->get_program_point()) != pp) {
            // Check if pp is in the if branch
            std::set<PP> pp_if_branch = program_points(if_stmt->if_branch);
            bool pp_in_if_branch = SetOps::is_value_in_set(pp, pp_if_branch);
            if (pp_in_if_branch) return get_elementary_block(if_stmt->if_branch, pp);

            // Check if pp is in the else branch
            std::set<PP> pp_else_branch = program_points(if_stmt->else_branch);
            bool pp_in_else_branch = SetOps::is_value_in_set(pp, pp_else_branch);
            if (pp_in_else_branch) return get_elementary_block(if_stmt->else_branch, pp);
        }

    } else if (auto while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        if (*(while_stmt->cond->get_program_point()) == pp)
            return while_stmt->cond;
        else
            return get_elementary_block(while_stmt->body, pp);

    } else if (auto seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        // Check if pp is in the first statement
        std::set<PP> pp_stmt_1 = program_points(seq_comp_stmt->stmt_1);
        bool pp_in_stmt_1 = SetOps::is_value_in_set(pp, pp_stmt_1);
        if (pp_in_stmt_1) return get_elementary_block(seq_comp_stmt->stmt_1, pp);

        // Check if pp is in the second statement
        std::set<PP> pp_stmt_2 = program_points(seq_comp_stmt->stmt_2);
        bool pp_in_stmt_2 = SetOps::is_value_in_set(pp, pp_stmt_2);
        if (pp_in_stmt_2) return get_elementary_block(seq_comp_stmt->stmt_2, pp);

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

PP initial_program_point(const std::shared_ptr<Stmt> &stmt)
{
    if (auto ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return *(ass_stmt->get_program_point());

    } else if (auto skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return *(skip_stmt->get_program_point());

    } else if (auto if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        return *(if_stmt->cond->get_program_point());

    } else if (auto while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        return *(while_stmt->cond->get_program_point());

    } else if (auto seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        return initial_program_point(seq_comp_stmt->stmt_1);

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

bool is_program_point_initial(const std::shared_ptr<Stmt> &stmt, const PP &pp)
{
    return pp == initial_program_point(stmt);
}

std::set<PP> final_program_points(const std::shared_ptr<Stmt> &stmt)
{
    if (auto ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return {*(ass_stmt->get_program_point())};

    } else if (auto skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return {*(skip_stmt->get_program_point())};

    } else if (auto if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        std::set<PP> if_branch_set = final_program_points(if_stmt->if_branch);
        std::set<PP> else_branch_set = final_program_points(if_stmt->else_branch);
        return SetOps::join_sets(if_branch_set, else_branch_set);

    } else if (auto while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        return {*(while_stmt->cond->get_program_point())};

    } else if (auto seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        return final_program_points(seq_comp_stmt->stmt_2);

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

bool is_program_point_final(const std::shared_ptr<Stmt> &stmt, const PP &pp)
{
    std::set<PP> pp_stmt = program_points(stmt);
    bool pp_in_stmt = SetOps::is_value_in_set(pp, pp_stmt);
    if (pp_in_stmt) return true;

    return false;
}

std::set<std::pair<PP, PP>> control_flow(const std::shared_ptr<Stmt> &stmt)
{
    if (auto ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return {};

    } else if (auto skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return {};

    } else if (auto if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        std::set<std::pair<PP, PP>> cf_if_branch = control_flow(if_stmt->if_branch);
        std::set<std::pair<PP, PP>> cf_else_branch = control_flow(if_stmt->else_branch);
        std::set<std::pair<PP, PP>> res_branches = SetOps::join_sets(cf_if_branch, cf_else_branch);

        PP init_pp_if_branch = initial_program_point(if_stmt->if_branch);
        PP init_pp_else_branch = initial_program_point(if_stmt->else_branch);
        std::set<std::pair<PP, PP>> if_flow = {
            std::make_pair(*(if_stmt->cond->get_program_point()), init_pp_if_branch),
            std::make_pair(*(if_stmt->cond->get_program_point()), init_pp_else_branch)
        };

        return SetOps::join_sets(res_branches, if_flow);

    } else if (auto while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        std::set<std::pair<PP, PP>> cf_body = control_flow(while_stmt->body);
        PP init_pp_body = initial_program_point(while_stmt->body);
        std::set<PP> final_pp_body = final_program_points(while_stmt->body);
        std::set<std::pair<PP, PP>> while_flow_start = {
            std::make_pair(*(while_stmt->cond->get_program_point()), init_pp_body)
        };
        std::set<std::pair<PP, PP>> while_flow_end;
        for(const auto &final_pp: final_pp_body)
            while_flow_end.insert(std::make_pair(final_pp, *(while_stmt->cond->get_program_point())));

        return SetOps::join_sets(cf_body, SetOps::join_sets(while_flow_start, while_flow_end));

    } else if (auto seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        std::set<std::pair<PP, PP>> cf_stmt_1 = control_flow(seq_comp_stmt->stmt_1);
        std::set<std::pair<PP, PP>> cf_stmt_2 = control_flow(seq_comp_stmt->stmt_2);
        std::set<std::pair<PP, PP>> res_stmts = SetOps::join_sets(cf_stmt_1, cf_stmt_2);

        std::set<PP> final_stmt_1 = final_program_points(seq_comp_stmt->stmt_1);
        PP init_stmt_2 = initial_program_point(seq_comp_stmt->stmt_2);

        std::set<std::pair<PP, PP>> seq_flow;
        for(const auto &final_pp: final_stmt_1)
            seq_flow.insert(std::make_pair(final_pp, init_stmt_2));

        return SetOps::join_sets(res_stmts, seq_flow);

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

bool has_isolated_entries(const std::shared_ptr<Stmt> &stmt)
{
    std::set<std::pair<PP, PP>> cf = control_flow(stmt);
    PP init_pp = initial_program_point(stmt);

    for (const auto &pp: program_points(stmt)) {
        auto pp_pair = std::make_pair(pp, init_pp);
        if (SetOps::is_value_in_set(pp_pair, cf)) return false;
    }

    return true;
}

bool has_isolated_exits(const std::shared_ptr<Stmt> &stmt)
{
    std::set<std::pair<PP, PP>> cf = control_flow(stmt);
    std::set<PP> final_pps = final_program_points(stmt);

    for (const auto &edge: cf) {
        if (SetOps::is_value_in_set(edge.first, final_pps)) return false;
    }

    return true;
}
