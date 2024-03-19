#include "analysis_utils.hpp"

/*
 * Compute the set of free variables in the given AExp.
 */
std::set<Var> free_variables_a_exp(const std::shared_ptr<AExp> &a_exp) noexcept
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
        throw std::runtime_error("No valid AExp!");
    }
}

/*
 * Compute the set of free variables in the given BExp.
 */
std::set<Var> free_variables_b_exp(const std::shared_ptr<BExp> &b_exp) noexcept {
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
        throw std::runtime_error("No valid BExp!");
    }
}

/*
 * Compute the set of free variables in the given Stmt.
 */
std::set<Var> free_variables_stmt(const std::shared_ptr<Stmt> &stmt) noexcept {
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
        throw std::runtime_error("No valid Stmt!");
    }
}

std::set<PP> program_points(const std::shared_ptr<Stmt> &stmt) noexcept
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
        throw std::runtime_error("No valid Stmt!");
    }
}