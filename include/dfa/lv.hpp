#pragma once

#include <vector>
#include <set>

#include "ast.hpp"
#include "analysis_utils.hpp"


/*
 * Live Variable Analysis (LV-Analysis) can be used to check whether
 * computed values are actually used. If values are unused but still
 * assigned to variables, these assignments are redundant.
 *
 * LV-Analysis is a backwards analysis which is clear from the
 * definitions of the functions entry and exit.
 *
 * Needs a program as input, as well as the ability to compute
 * free variables, control flow, program points, and final
 * program points of the program.
 */
class LiveVariableAnalysis {
    // Statement
    std::shared_ptr<Stmt> stmt;

    // Program points
    std::set<PP> pps;

    // Control flow
    std::set<std::pair<PP, PP>> cf;

    // Final program points
    std::set<PP> final_pps;

    // Number of program points
    unsigned int n;

public:
    /*
     * Initialize the members using the utility functions.
     * LV-analysis needs the program points, the control flow, and the final program points.
     */
    explicit LiveVariableAnalysis(const std::shared_ptr<Stmt> &stmt):
            stmt(stmt), pps(program_points(stmt)), cf(control_flow(stmt)), final_pps(final_program_points(stmt))
    {
        this->n = pps.size();

        this->check_analysis_constraints();
    }

    /*
     * The function compute that calculates the live variables at each program point.
     */
    [[nodiscard]] std::vector<std::set<Var>> compute() const;

    /*
     * The function F_LV that makes one analysis iteration.
     * vec = F_LV(vec)
     */
    [[nodiscard]] std::vector<std::set<Var>> F_LV(const std::vector<std::set<Var>> &v) const;

    /*
     * Checks whether a fixpoint is reached, i.e. the vectors contain the same sets with the same elements.
     */

    [[nodiscard]] static bool fixpoint_reached(const std::vector<std::set<Var>> &vec_1,
                                               const std::vector<std::set<Var>> &vec_2);

    /*
     * Injective function that maps indices {0, ... , n-1} to program points.
     */
    [[nodiscard]] PP f(unsigned int i) const;

    /*
     * Generates a variable x for an elementary block iff x is read by this elementary block.
     */
    [[nodiscard]] std::set<Var> gen_LV(const std::shared_ptr<Block> &block) const;

    /*
     * Kills a variable x for an elementary block iff x might be modified by this elementary block.
     */
    [[nodiscard]] std::set<Var> kill_LV(const std::shared_ptr<Block> &block) const;

private:
    /*
     * This function checks whether the constraints for the LV-analysis are met,
     * i.e. if the given statement/program has isolated exits.
     * If not, a simple skip statement is added at the end to ensure it works correctly.
     */
    void check_analysis_constraints();
};
