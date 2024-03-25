#include "analysis_utils.hpp"

/*
 * Compute the set of free variables in the given arithmetic expression.
 */
std::set<Var> free_variables_a_exp(const std::shared_ptr<AExp> &a_exp)
{
    // Todo: please give pattern matching ... seems not very elegant this way?!
    if (Var* var = dynamic_cast<Var*>(a_exp.get())) {
        return {*var};
    } else if (Num* num = dynamic_cast<Num*>(a_exp.get())) {
        return {};
    } else if (OpA* op_A = dynamic_cast<OpA*>(a_exp.get())) {
        std::set<Var> res;
        std::set<Var> lhs_set = free_variables_a_exp(op_A->lhs);
        std::set<Var> rhs_set = free_variables_a_exp(op_A->rhs);
        std::set_union(
            lhs_set.begin(), lhs_set.end(),
            rhs_set.begin(), rhs_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else {
        throw std::runtime_error("Unknown AExp!");
    }
}

/*
 * Compute the set of free variables in the given boolean expression.
 */
std::set<Var> free_variables_b_exp(const std::shared_ptr<BExp> &b_exp)
{
    if (Bool *b = dynamic_cast<Bool *>(b_exp.get())) {
        return {};
    } else if (Not *n = dynamic_cast<Not *>(b_exp.get())) {
        return free_variables_b_exp(n->b_exp);
    } else if (OpR *op_R = dynamic_cast<OpR *>(b_exp.get())) {
        std::set<Var> res;
        std::set<Var> lhs_set = free_variables_a_exp(op_R->lhs);
        std::set<Var> rhs_set = free_variables_a_exp(op_R->rhs);
        std::set_union(
                lhs_set.begin(), lhs_set.end(),
                rhs_set.begin(), rhs_set.end(),
                std::inserter(res, res.begin())
        );
        return res;
    } else if (OpB *op_B = dynamic_cast<OpB *>(b_exp.get())) {
        std::set<Var> res;
        std::set<Var> lhs_set = free_variables_b_exp(op_B->lhs);
        std::set<Var> rhs_set = free_variables_b_exp(op_B->rhs);
        std::set_union(
                lhs_set.begin(), lhs_set.end(),
                rhs_set.begin(), rhs_set.end(),
                std::inserter(res, res.begin())
        );
        return res;
    } else {
        throw std::runtime_error("Unknown BExp!");
    }
}

/*
 * Compute the set of free variables in the given program/statement.
 */
std::set<Var> free_variables_stmt(const std::shared_ptr<Stmt> &stmt)
{
    if (Ass *ass_stmt = dynamic_cast<Ass *>(stmt.get())) {
        std::set<Var> res;
        std::set<Var> fst_set = {*(ass_stmt->var)};
        std::set<Var> snd_set = free_variables_a_exp(ass_stmt->a_exp);
        std::set_union(
            fst_set.begin(), fst_set.end(),
            snd_set.begin(), snd_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else if (Skip *skip_stmt = dynamic_cast<Skip *>(stmt.get())) {
        return {};
    } else if (If *if_stmt = dynamic_cast<If *>(stmt.get())) {
        std::set<Var> res;
        std::set<Var> cond_set = free_variables_b_exp(if_stmt->cond->b_exp);
        std::set<Var> if_branch_set = free_variables_stmt(if_stmt->if_branch);
        std::set<Var> else_branch_set = free_variables_stmt(if_stmt->else_branch);
        std::set<Var> branches_set;
        std::set_union(
            if_branch_set.begin(), if_branch_set.end(),
            else_branch_set.begin(), else_branch_set.end(),
            std::inserter(branches_set, branches_set.begin())
        );
        std::set_union(
            cond_set.begin(), cond_set.end(),
            branches_set.begin(), branches_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else if (While *while_stmt = dynamic_cast<While *>(stmt.get())) {
        std::set<Var> res;
        std::set<Var> body_set = free_variables_stmt(while_stmt->body);
        std::set<Var> cond_set = free_variables_b_exp(while_stmt->cond->b_exp);
        std::set_union(
            body_set.begin(), body_set.end(),
            cond_set.begin(), cond_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else if (SeqComp *seq_comp_stmt = dynamic_cast<SeqComp *>(stmt.get())) {
        std::set<Var> res;
        std::set<Var> stmt_1_set = free_variables_stmt(seq_comp_stmt->stmt_1);
        std::set<Var> stmt_2_set = free_variables_stmt(seq_comp_stmt->stmt_2);
        std::set_union(
            stmt_1_set.begin(), stmt_1_set.end(),
            stmt_2_set.begin(), stmt_2_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

/*
 * Compute the set of program points within a program/statement.
 */
std::set<PP> program_points(const std::shared_ptr<Stmt> &stmt)
{
    if (Ass *ass_stmt = dynamic_cast<Ass *>(stmt.get())) {
        return {*(ass_stmt->get_program_point())};
    } else if (Skip *skip_stmt = dynamic_cast<Skip *>(stmt.get())) {
        return {*(skip_stmt->get_program_point())};
    } else if (If *if_stmt = dynamic_cast<If *>(stmt.get())) {
        std::set<PP> res;
        std::set<PP> cond_set = { *(if_stmt->cond->get_program_point()) };
        std::set<PP> if_branch_set = program_points(if_stmt->if_branch);
        std::set<PP> else_branch_set = program_points(if_stmt->else_branch);
        std::set<PP> branches_set;
        std::set_union(
            if_branch_set.begin(), if_branch_set.end(),
            else_branch_set.begin(), else_branch_set.end(),
            std::inserter(branches_set, branches_set.begin())
        );
        std::set_union(
            cond_set.begin(), cond_set.end(),
            branches_set.begin(), branches_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else if (While *while_stmt = dynamic_cast<While *>(stmt.get())) {
        std::set<PP> res;
        std::set<PP> cond_set = { *(while_stmt->cond->get_program_point()) };
        std::set<PP> body_set = program_points(while_stmt->body);
        std::set_union(
            cond_set.begin(), cond_set.end(),
            body_set.begin(), body_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else if (SeqComp *seq_comp_stmt = dynamic_cast<SeqComp *>(stmt.get())) {
        std::set<PP> res;
        std::set<PP> stmt_1_set = program_points(seq_comp_stmt->stmt_1);
        std::set<PP> stmt_2_set = program_points(seq_comp_stmt->stmt_2);
        std::set_union(
            stmt_1_set.begin(), stmt_1_set.end(),
            stmt_2_set.begin(), stmt_2_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

/*
 * Check whether a program/statement is well-formed, i.e. each program point occurs at most once.
 */
bool is_program_well_formed(const std::shared_ptr<Stmt> &stmt)
{
    if (Ass *ass_stmt = dynamic_cast<Ass *>(stmt.get())) {
        return true;
    } else if (Skip *skip_stmt = dynamic_cast<Skip *>(stmt.get())) {
        return true;
    } else if (If *if_stmt = dynamic_cast<If *>(stmt.get())) {
        bool wf_if_branch = is_program_well_formed(if_stmt->if_branch);
        bool wf_else_branch = is_program_well_formed(if_stmt->else_branch);

        std::set<PP> pp_if_branch = program_points(if_stmt->if_branch);
        std::set<PP> pp_else_branch = program_points(if_stmt->else_branch);
        std::set<PP> branch_intersection;
        std::set_intersection(
            pp_if_branch.begin(), pp_if_branch.end(),
            pp_else_branch.begin(), pp_else_branch.end(),
            std::inserter(branch_intersection, branch_intersection.begin())
        );
        std::set<PP> branch_union;
        std::set_union(
                pp_if_branch.begin(), pp_if_branch.end(),
                pp_else_branch.begin(), pp_else_branch.end(),
                std::inserter(branch_union, branch_union.begin())
        );
        bool pp_not_in_branches = branch_union.find(*(if_stmt->cond->get_program_point())) == branch_union.end();

        return wf_if_branch && wf_else_branch && branch_intersection.empty() && pp_not_in_branches;
    } else if (While *while_stmt = dynamic_cast<While *>(stmt.get())) {
        bool wf_body = is_program_well_formed(while_stmt->body);
        std::set<PP> pp_body = program_points(while_stmt->body);
        bool pp_not_in_body = pp_body.find(*(while_stmt->cond->get_program_point())) == pp_body.end();

        return wf_body && pp_not_in_body;
    } else if (SeqComp *seq_comp_stmt = dynamic_cast<SeqComp *>(stmt.get())) {
        std::set<PP> pp_stmt_1 = program_points(seq_comp_stmt->stmt_1);
        std::set<PP> pp_stmt_2 = program_points(seq_comp_stmt->stmt_2);
        bool wf_stmt_1 = is_program_well_formed(seq_comp_stmt->stmt_1);
        bool wf_stmt_2 = is_program_well_formed(seq_comp_stmt->stmt_2);
        std::set<PP> stmt_intersection;
        std::set_intersection(
                pp_stmt_1.begin(), pp_stmt_1.end(),
                pp_stmt_2.begin(), pp_stmt_2.end(),
                std::inserter(stmt_intersection, stmt_intersection.begin())
        );

        return wf_stmt_1 && wf_stmt_2 && stmt_intersection.empty();
    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

/*
 * Compute the number of occurrences of the program point in the program/statement.
 */
unsigned int program_point_occurrences(const std::shared_ptr<Stmt> &stmt, const PP &pp)
{
    if (Ass *ass_stmt = dynamic_cast<Ass *>(stmt.get())) {
        return (*(ass_stmt->get_program_point()) == pp) ? 1 : 0;
    } else if (Skip *skip_stmt = dynamic_cast<Skip *>(stmt.get())) {
        return (*(skip_stmt->get_program_point()) == pp) ? 1 : 0;
    } else if (If *if_stmt = dynamic_cast<If *>(stmt.get())) {
        unsigned int pp_occurrences_if_branch = program_point_occurrences(if_stmt->if_branch, pp);
        unsigned int pp_occurrences_else_branch = program_point_occurrences(if_stmt->else_branch, pp);
        if (*(if_stmt->cond->get_program_point()) == pp)
            return pp_occurrences_if_branch + pp_occurrences_else_branch + 1;
        else
            return pp_occurrences_if_branch + pp_occurrences_else_branch;
    } else if (While *while_stmt = dynamic_cast<While *>(stmt.get())) {
        unsigned int pp_occurrences_body = program_point_occurrences(while_stmt->body, pp);
        if (*(while_stmt->cond->get_program_point()) == pp)
            return pp_occurrences_body + 1;
        else
            return pp_occurrences_body;
    } else if (SeqComp *seq_comp_stmt = dynamic_cast<SeqComp *>(stmt.get())) {
        unsigned int pp_occurrences_stmt_1 = program_point_occurrences(seq_comp_stmt->stmt_1, pp);
        unsigned int pp_occurrences_stmt_2 = program_point_occurrences(seq_comp_stmt->stmt_2, pp);
        return pp_occurrences_stmt_1 + pp_occurrences_stmt_2;
    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

/*
 * Compute set of elementary blocks in given program/statement.
 */
std::set<std::shared_ptr<Block>> elementary_blocks(const std::shared_ptr<Stmt> &stmt)
{
    if (std::shared_ptr<Ass> ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return {ass_stmt};
    } else if (std::shared_ptr<Skip> skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return {skip_stmt};
    } else if (std::shared_ptr<If> if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        std::set<std::shared_ptr<Block>> res;
        std::set<std::shared_ptr<Block>> cond_set = {if_stmt->cond};
        std::set<std::shared_ptr<Block>> if_branch_set = elementary_blocks(if_stmt->if_branch);
        std::set<std::shared_ptr<Block>> else_branch_set = elementary_blocks(if_stmt->else_branch);
        std::set<std::shared_ptr<Block>> branches_set;
        std::set_union(
            if_branch_set.begin(), if_branch_set.end(),
            else_branch_set.begin(), else_branch_set.end(),
            std::inserter(branches_set, branches_set.begin())
        );
        std::set_union(
            cond_set.begin(), cond_set.end(),
            branches_set.begin(), branches_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else if (std::shared_ptr<While> while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        std::set<std::shared_ptr<Block>> res;
        std::set<std::shared_ptr<Block>> cond_set = {while_stmt->cond};
        std::set<std::shared_ptr<Block>> body_set = elementary_blocks(while_stmt->body);
        std::set_union(
            cond_set.begin(), cond_set.end(),
            body_set.begin(), body_set.end(),
            std::inserter(res, res.begin())
        );
        return res;
    } else if (std::shared_ptr<SeqComp> seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        std::set<std::shared_ptr<Block>> res;
        std::set<std::shared_ptr<Block>> stmt_1_set = elementary_blocks(seq_comp_stmt->stmt_1);
        std::set<std::shared_ptr<Block>> stmt_2_set = elementary_blocks(seq_comp_stmt->stmt_2);
        std::set_union(
                stmt_1_set.begin(), stmt_1_set.end(),
                stmt_2_set.begin(), stmt_2_set.end(),
                std::inserter(res, res.begin())
        );
        return res;
    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

/*
 * Return the respective block of a given stmt at pp.
 */
std::shared_ptr<Block> get_elementary_block(const std::shared_ptr<Stmt> &stmt, const PP &pp)
{
    if (std::shared_ptr<Ass> ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        if (*(ass_stmt->get_program_point()) == pp) return ass_stmt;

    } else if (std::shared_ptr<Skip> skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        if (*(skip_stmt->get_program_point()) == pp) return skip_stmt;

    } else if (std::shared_ptr<If> if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        if (*(if_stmt->cond->get_program_point()) == pp)
            return if_stmt->cond;
        else if (*(if_stmt->cond->get_program_point()) != pp) {
            // Check if pp is in the if branch
            std::set<PP> pp_if_branch = program_points(if_stmt->if_branch);
            bool pp_in_if_branch = pp_if_branch.find(pp) != pp_if_branch.end();
            if (pp_in_if_branch) return get_elementary_block(if_stmt->if_branch, pp);

            // Check if pp is in the else branch
            std::set<PP> pp_else_branch = program_points(if_stmt->else_branch);
            bool pp_in_else_branch = pp_else_branch.find(pp) != pp_else_branch.end();
            if (pp_in_else_branch) return get_elementary_block(if_stmt->else_branch, pp);
        }

    } else if (std::shared_ptr<While> while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        if (*(while_stmt->cond->get_program_point()) == pp)
            return while_stmt->cond;
        else
            return get_elementary_block(while_stmt->body, pp);

    } else if (std::shared_ptr<SeqComp> seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        // Check if pp is in the first statement
        std::set<PP> pp_stmt_1 = program_points(seq_comp_stmt->stmt_1);
        bool pp_in_stmt_1 = pp_stmt_1.find(pp) != pp_stmt_1.end();
        if (pp_in_stmt_1) return get_elementary_block(seq_comp_stmt->stmt_1, pp);

        // Check if pp is in the second statement
        std::set<PP> pp_stmt_2 = program_points(seq_comp_stmt->stmt_2);
        bool pp_in_stmt_2 = pp_stmt_2.find(pp) != pp_stmt_2.end();
        if (pp_in_stmt_2) return get_elementary_block(seq_comp_stmt->stmt_2, pp);

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

/*
 * Return the initial program point of a given stmt.
 */
PP initial_program_point(const std::shared_ptr<Stmt> &stmt)
{
    if (std::shared_ptr<Ass> ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return *(ass_stmt->get_program_point());
    } else if (std::shared_ptr<Skip> skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return *(skip_stmt->get_program_point());
    } else if (std::shared_ptr<If> if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        return *(if_stmt->cond->get_program_point());
    } else if (std::shared_ptr<While> while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        return *(while_stmt->cond->get_program_point());
    } else if (std::shared_ptr<SeqComp> seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        return initial_program_point(seq_comp_stmt->stmt_1);
    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

/*
 * Checks whether the given program point is the initial program point in the given statement.
 */
bool is_program_point_initial(const std::shared_ptr<Stmt> &stmt, const PP &pp)
{
    return pp == initial_program_point(stmt);
}

/*
 * Return the set of final program points of a given stmt.
 */
std::set<PP> final_program_points(const std::shared_ptr<Stmt> &stmt)
{
    if (std::shared_ptr<Ass> ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return {*(ass_stmt->get_program_point())};
    } else if (std::shared_ptr<Skip> skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return {*(skip_stmt->get_program_point())};
    } else if (std::shared_ptr<If> if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        std::set<PP> res;
        std::set<PP> if_branch_set = final_program_points(if_stmt->if_branch);
        std::set<PP> else_branch_set = final_program_points(if_stmt->else_branch);
        std::set_union(
                if_branch_set.begin(), if_branch_set.end(),
                else_branch_set.begin(), else_branch_set.end(),
                std::inserter(res, res.begin())
        );
        return res;
    } else if (std::shared_ptr<While> while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        return {*(while_stmt->cond->get_program_point())};
    } else if (std::shared_ptr<SeqComp> seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        return final_program_points(seq_comp_stmt->stmt_2);
    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

/*
 * Checks whether the given program point is the initial program point in the given statement.
 */
bool is_program_point_final(const std::shared_ptr<Stmt> &stmt, const PP &pp)
{
    std::set<PP> pp_stmt = program_points(stmt);
    bool pp_in_stmt = pp_stmt.find(pp) != pp_stmt.end();
    if (pp_in_stmt) return true;

    return false;
}

/*
 * Return the control flow of a given stmt.
 */
std::set<std::pair<PP, PP>> control_flow(const std::shared_ptr<Stmt> &stmt)
{
    if (std::shared_ptr<Ass> ass_stmt = std::dynamic_pointer_cast<Ass>(stmt)) {
        return {};
    } else if (std::shared_ptr<Skip> skip_stmt = std::dynamic_pointer_cast<Skip>(stmt)) {
        return {};
    } else if (std::shared_ptr<If> if_stmt = std::dynamic_pointer_cast<If>(stmt)) {
        std::set<std::pair<PP, PP>> res_branches;
        std::set<std::pair<PP, PP>> cf_if_branch = control_flow(if_stmt->if_branch);
        std::set<std::pair<PP, PP>> cf_else_branch = control_flow(if_stmt->else_branch);
        std::set_union(
                cf_if_branch.begin(), cf_if_branch.end(),
                cf_else_branch.begin(), cf_else_branch.end(),
                std::inserter(res_branches, res_branches.begin())
        );

        PP init_pp_if_branch = initial_program_point(if_stmt->if_branch);
        PP init_pp_else_branch = initial_program_point(if_stmt->else_branch);
        std::set<std::pair<PP, PP>> if_flow = {
            std::make_pair(*(if_stmt->cond->get_program_point()), init_pp_if_branch),
            std::make_pair(*(if_stmt->cond->get_program_point()), init_pp_else_branch)
        };

        std::set<std::pair<PP, PP>> res;
        std::set_union(
                res_branches.begin(), res_branches.end(),
                if_flow.begin(), if_flow.end(),
                std::inserter(res, res.begin())
        );
        return res;

    } else if (std::shared_ptr<While> while_stmt = std::dynamic_pointer_cast<While>(stmt)) {
        std::set<std::pair<PP, PP>> cf_body = control_flow(while_stmt->body);
        PP init_pp_body = initial_program_point(while_stmt->body);
        std::set<PP> final_pp_body = final_program_points(while_stmt->body);
        std::set<std::pair<PP, PP>> while_flow_start = {
            std::make_pair(*(while_stmt->cond->get_program_point()), init_pp_body)
        };
        std::set<std::pair<PP, PP>> while_flow_end;
        for(const auto &final_pp: final_pp_body)
            while_flow_end.insert(std::make_pair(final_pp, *(while_stmt->cond->get_program_point())));

        std::set<std::pair<PP, PP>> res_while;
        std::set_union(
            while_flow_start.begin(), while_flow_start.end(),
            while_flow_end.begin(), while_flow_end.end(),
            std::inserter(res_while, res_while.begin())
        );

        std::set<std::pair<PP, PP>> res;
        std::set_union(
            cf_body.begin(), cf_body.end(),
            res_while.begin(), res_while.end(),
            std::inserter(res, res.begin())
        );

        return res;

    } else if (std::shared_ptr<SeqComp> seq_comp_stmt = std::dynamic_pointer_cast<SeqComp>(stmt)) {
        std::set<std::pair<PP, PP>> res_stmts;
        std::set<std::pair<PP, PP>> cf_stmt_1 = control_flow(seq_comp_stmt->stmt_1);
        std::set<std::pair<PP, PP>> cf_stmt_2 = control_flow(seq_comp_stmt->stmt_2);
        std::set_union(
            cf_stmt_1.begin(), cf_stmt_1.end(),
            cf_stmt_2.begin(), cf_stmt_2.end(),
            std::inserter(res_stmts, res_stmts.begin())
        );

        std::set<PP> final_stmt_1 = final_program_points(seq_comp_stmt->stmt_1);
        PP init_stmt_2 = initial_program_point(seq_comp_stmt->stmt_2);

        std::set<std::pair<PP, PP>> seq_flow;
        for(const auto &final_pp: final_stmt_1)
            seq_flow.insert(std::make_pair(final_pp, init_stmt_2));

        std::set<std::pair<PP, PP>> res;
        std::set_union(
            res_stmts.begin(), res_stmts.end(),
            seq_flow.begin(), seq_flow.end(),
            std::inserter(res, res.begin())
        );
        return res;

    } else {
        throw std::runtime_error("Unknown Stmt!");
    }
}

/*
 * Checks whether a program has an isolated entry, i.e., there is no (pp, init(stmt)) in flow(stmt).
 */
bool has_isolated_entries(const std::shared_ptr<Stmt> &stmt)
{
    std::set<std::pair<PP, PP>> cf = control_flow(stmt);
    PP init_pp = initial_program_point(stmt);

    for(const auto &pp: program_points(stmt)) {
        auto pp_pair = std::make_pair(pp, init_pp);
        if (cf.find(pp_pair) != cf.end()) return false;
    }

    return true;
}

/*
 * Checks whether a program has isolated exits, i.e., there is no f_pp in final(stmt) with (f_pp, pp).
 */
bool has_isolated_exits(const std::shared_ptr<Stmt> &stmt)
{
    std::set<std::pair<PP, PP>> cf = control_flow(stmt);
    std::set<PP> final_pps = final_program_points(stmt);

    for(const auto &edge: cf) {
        if (final_pps.find(edge.first) != final_pps.end()) return false;
    }

    return true;
}

