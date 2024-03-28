#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include "test/test_analysis_utils.hpp"
#include "dfa/lv.hpp"


std::string read_program(char* argv[], const std::string &filename)
{
    std::filesystem::path exe_path = std::filesystem::canonical(std::filesystem::path(argv[0]));
    std::filesystem::path file_path = exe_path.parent_path().parent_path() / filename;

    std::string program;
    try {
        // Read example program in the simple while language
        WLangReader wlr(file_path);
        program = wlr.read_file();
    } catch(const std::exception &e) { return ""; }

    return program;
}

void test()
{
    test_free_variables_a_exp_trivial(false);
    test_free_variables_a_exp(false);
    test_free_variables_a_exp_complex(false);
    test_free_variables_stmt_complex(false);
    test_program_points_complex(false);
    test_is_program_well_formed_complex(false);
    test_program_point_occurrences(false);
    test_elementary_blocks(false);
    test_get_elementary_block(false);
    test_initial_program_point(false);
    test_is_program_point_initial(false);
    test_final_program_points(false);
    test_is_program_point_final(false);
    test_control_flow(false);
    test_has_isolated_entries(false);
    test_has_isolated_exits(false);
}

void run(char* argv[])
{
    // Read program from file
    std::string program = read_program(argv, "./test_programs/ex_1_4_b_program.wlang");

    // Lexer to create tokens
    std::vector<Token> tokens = Lexer::tokenize(program);

    // Parser to create AST
    Parser parser = Parser(tokens);
    std::shared_ptr<Stmt> stmt = parser.parse();
    //stmt->print_node(0);


    // LV-analysis
    auto lv = LiveVariableAnalysis(stmt);
    std::vector<std::set<Var>> result = lv.compute();
    lv.print_result(result);

}

int main(int argc, char* argv[])
{
    if (argc < 1) { std::cerr << "No command-line arguments provided." << std::endl; return 1; }

    //test();
    run(argv);

    return 0;
}


