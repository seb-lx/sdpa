#pragma once

#include "utils.hpp"
#include "ast.hpp"
#include "dfa_utils.hpp"


/**
 * Live Variable Analysis (LV-Analysis) can be used to check whether computed values are actually used.
 * If values are unused but still assigned to variables, these assignments are redundant.
 * 
 * LV-Analysis is a backwards analysis which is clear from the definitions of the functions entry and exit.
 * 
 * Needs a program as input, as well as the ability to compute free variables, control flow, program points, 
 * and final program points of the program.
 */ 
class LiveVariableAnalysis {
private:
    const Stmt* stmt_;                  // Statement
    std::set<PP> pps_;                  // Program points
    CFG cf_;                            // Control flow
    std::set<PP> final_pps_;            // Final program points
    unsigned int n_;                    // Number of program points

public:
    /*
     * Initialize the members using the utility functions.
     * LV-analysis needs the program points, the control flow, and the final program points.
     */
    explicit LiveVariableAnalysis(const Stmt* stmt): stmt_{stmt}
    {
        // Check if program is well-formed
        if (!dfa_utils::well_formed(stmt)) throw std::runtime_error("Program is not well-formed!");

        // Check if program has isolated exits which is needed to perform LV analysis
        //  note: since unique ptrs are used its hard to add another statement without moving
        //        needs to be done manually atm
        if (!dfa_utils::has_isolated_exits(stmt)) throw std::runtime_error("Program does not have isolated exits!");

        // calculate set of pps and its size, init control-flow and final program points 
        init(stmt);
    }

    /*
     * The function compute that calculates the live variables at each program point.
     */
    [[nodiscard]] auto compute() const -> LiveVariablesVec;

    /*
     * The function F_LV that makes one analysis iteration.
     * vec = F_LV(vec)
     */
    [[nodiscard]] auto F_LV(const LiveVariablesVec& v) const -> LiveVariablesVec;

    /*
     * Checks whether a fixpoint is reached, i.e. the vectors contain the same sets with the same elements.
     */
    [[nodiscard]] static bool fixpoint_reached(const LiveVariablesVec& vec_1, const LiveVariablesVec& vec_2);
                                               

    /*
     * Injective function that maps indices {0, ... , n-1} to program points.
     */
    [[nodiscard]] PP f(unsigned int i) const;

    /*
     * Generates a variable x for an elementary block iff x is read by this elementary block.
     */
    [[nodiscard]] auto gen_LV(const Block* block) const -> LiveVariables;

    /*
     * Kills a variable x for an elementary block iff x might be modified by this elementary block.
     */
    [[nodiscard]] auto kill_LV(const Block* block) const -> LiveVariables;

    /*
     * Prints the result to cout.
     */
    static void print_result(const LiveVariablesVec& res);

private:
    /*
     * This function checks whether the constraints for the LV-analysis are met,
     * i.e. if the given statement/program has isolated exits.
     * If not, a simple skip statement is added at the end to ensure it works correctly.
     */
    //void check_and_enforce_analysis_constraints(const std::unique_ptr<Stmt>& stmt);

    /*
     * Calculates and initializes all information that is needed for the LV-analysis.
     */
    void init(const Stmt* stmt);
};