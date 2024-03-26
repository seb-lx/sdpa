#include "test_analysis_utils.hpp"


namespace WLangTestPrograms {
    std::shared_ptr<Stmt> factorial_stmt =
            std::make_shared<SeqComp>(
                    std::make_shared<Ass>(
                            std::make_shared<Var>("y"),
                            std::make_shared<Var>("x"),
                            std::make_shared<PP>(1)
                    ),
                    std::make_shared<SeqComp>(
                            std::make_shared<Ass>(
                                    std::make_shared<Var>("z"),
                                    std::make_shared<Num>(1),
                                    std::make_shared<PP>(2)
                            ),
                            std::make_shared<SeqComp>(
                                    std::make_shared<While>(
                                            std::make_shared<Cond>(
                                                    std::make_shared<Greater>(
                                                            std::make_shared<Var>("y"),
                                                            std::make_shared<Num>(1)
                                                    ),
                                                    std::make_shared<PP>(3)
                                            ),
                                            std::make_shared<SeqComp>(
                                                    std::make_shared<Ass>(
                                                            std::make_shared<Var>("z"),
                                                            std::make_shared<Mul>(
                                                                    std::make_shared<Var>("z"),
                                                                    std::make_shared<Var>("y")
                                                            ),
                                                            std::make_shared<PP>(4)
                                                    ),
                                                    std::make_shared<Ass>(
                                                            std::make_shared<Var>("y"),
                                                            std::make_shared<Sub>(
                                                                    std::make_shared<Var>("y"),
                                                                    std::make_shared<Num>(1)
                                                            ),
                                                            std::make_shared<PP>(5)
                                                    )
                                            )
                                    ),
                                    std::make_shared<Ass>(
                                            std::make_shared<Var>("y"),
                                            std::make_shared<Num>(0),
                                            std::make_shared<PP>(6)
                                    )
                            )
                    )
            );
}


void print_test_result(bool b, const std::string &func_name)
{
    std::cout << "Test for " << func_name << (b ? ": Passed!" : ": Failed!") << std::endl;
}


void test_free_variables_a_exp_trivial(bool print)
{
    std::shared_ptr<AExp> a_exp = std::make_shared<Add>(
            std::make_shared<Num>(1),
            std::make_shared<Num>(1)
    );
    if(print) a_exp->print_node(0);

    std::set<Var> res = free_variables_a_exp(a_exp);
    std::set<Var> expected = {};
    print_test_result(SetOps::are_sets_equal(res, expected), __func__);
}

void test_free_variables_a_exp(bool print)
{
    std::shared_ptr<AExp> a_exp = std::make_shared<Add>(
            std::make_shared<Var>("a"),
            std::make_shared<Sub>(
                    std::make_shared<Num>(1), std::make_shared<Var>("b")
            )
    );
    if(print) a_exp->print_node(0);

    std::set<Var> res = free_variables_a_exp(a_exp);
    std::set<Var> expected = { Var("a"), Var("b") };
    print_test_result(SetOps::are_sets_equal(res, expected), __func__);
}

void test_free_variables_a_exp_complex(bool print)
{
    std::shared_ptr<AExp> a_exp = std::make_shared<Add>(
            std::make_shared<Add>(
                    std::make_shared<Add>(
                            std::make_shared<Mul>(std::make_shared<Var>("a"), std::make_shared<Var>("a")),
                            std::make_shared<Sub>(std::make_shared<Num>(1), std::make_shared<Var>("bB"))
                    ),
                    std::make_shared<Num>(1)
            ),
            std::make_shared<Add>(
                    std::make_shared<Mul>(std::make_shared<Var>("z"), std::make_shared<Var>("y")),
                    std::make_shared<Sub>(std::make_shared<Num>(1), std::make_shared<Var>("x"))
            )
    );
    if(print) a_exp->print_node(0);

    std::set<Var> res = free_variables_a_exp(a_exp);
    std::set<Var> expected = { Var("a"), Var("bB"), Var("x"), Var("y"), Var("z") };
    print_test_result(SetOps::are_sets_equal(res, expected), __func__);
}


void test_free_variables_stmt_complex(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    std::set<Var> res = free_variables_stmt(WLangTestPrograms::factorial_stmt);
    std::set<Var> expected = { Var("y"), Var("x"), Var("z") };
    print_test_result(SetOps::are_sets_equal(res, expected), __func__);
}

void test_program_points_complex(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    std::set<PP> res = program_points(WLangTestPrograms::factorial_stmt);
    std::set<PP> expected = { PP(1), PP(2), PP(3), PP(4), PP(5), PP(6) };
    print_test_result(SetOps::are_sets_equal(res, expected), __func__);
}

void test_is_program_well_formed_complex(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    bool res = is_program_well_formed(WLangTestPrograms::factorial_stmt);
    bool expected = true;
    print_test_result(res == expected, __func__);
}

void test_program_point_occurrences(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    unsigned int res = program_point_occurrences(WLangTestPrograms::factorial_stmt, PP(1));
    unsigned int expected = 1;
    print_test_result(res == expected, __func__);
}

void test_elementary_blocks(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    std::set<std::shared_ptr<Block>> res = elementary_blocks(WLangTestPrograms::factorial_stmt);
    std::set<std::shared_ptr<Block>> expected = {
            std::make_shared<Ass>(std::make_shared<Var>("y"), std::make_shared<Var>("x"), std::make_shared<PP>(1)),
            std::make_shared<Ass>(std::make_shared<Var>("z"), std::make_shared<Num>(1), std::make_shared<PP>(2)),
            std::make_shared<Cond>(
                    std::make_shared<Greater>(std::make_shared<Var>("y"), std::make_shared<Num>(1)),
                    std::make_shared<PP>(3)
            ),
            std::make_shared<Ass>(
                    std::make_shared<Var>("z"),
                    std::make_shared<Mul>(std::make_shared<Var>("z"), std::make_shared<Var>("y")),
                    std::make_shared<PP>(4)
            ),
            std::make_shared<Ass>(
                    std::make_shared<Var>("y"),
                    std::make_shared<Sub>(std::make_shared<Var>("y"), std::make_shared<Num>(1)),
                    std::make_shared<PP>(5)
            ),
            std::make_shared<Ass>(std::make_shared<Var>("y"), std::make_shared<Num>(0), std::make_shared<PP>(6))
    };

    print_test_result(SetOps::are_pointer_sets_equal(res, expected), __func__);
}

void test_get_elementary_block(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    std::shared_ptr<Block> res = get_elementary_block(WLangTestPrograms::factorial_stmt, PP(5));
    std::shared_ptr<Block> expected = std::make_shared<Ass>(
            std::make_shared<Var>("y"),
            std::make_shared<Sub>(std::make_shared<Var>("y"), std::make_shared<Num>(1)),
            std::make_shared<PP>(5)
    );

    print_test_result(*res == *expected, __func__);
}

void test_initial_program_point(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    PP res = initial_program_point(WLangTestPrograms::factorial_stmt);
    PP expected = PP(1);

    print_test_result(res == expected, __func__);
}

void test_is_program_point_initial(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    bool res = is_program_point_initial(WLangTestPrograms::factorial_stmt, PP(1));
    bool expected = true;

    print_test_result(res == expected, __func__);
}

void test_final_program_points(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    std::set<PP> res = final_program_points(WLangTestPrograms::factorial_stmt);
    std::set<PP> expected = { PP(6) };

    print_test_result(SetOps::are_sets_equal(res, expected), __func__);
}

void test_is_program_point_final(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    bool res = is_program_point_final(WLangTestPrograms::factorial_stmt, PP(6));
    bool expected = true;

    print_test_result(res == expected, __func__);
}

void test_control_flow(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    std::set<std::pair<PP, PP>> res = control_flow(WLangTestPrograms::factorial_stmt);
    std::set<std::pair<PP, PP>> expected = {
            std::make_pair(PP(1), PP(2)),
            std::make_pair(PP(2), PP(3)),
            std::make_pair(PP(3), PP(4)),
            std::make_pair(PP(4), PP(5)),
            std::make_pair(PP(5), PP(3)),
            std::make_pair(PP(3), PP(6)),
    };

    print_test_result(SetOps::are_sets_equal(res, expected), __func__);
}

void test_has_isolated_entries(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    bool res = has_isolated_entries(WLangTestPrograms::factorial_stmt);
    bool expected = true;

    print_test_result(res == expected, __func__);
}

void test_has_isolated_exits(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    bool res = has_isolated_exits(WLangTestPrograms::factorial_stmt);
    bool expected = true;

    print_test_result(res == expected, __func__);
}
