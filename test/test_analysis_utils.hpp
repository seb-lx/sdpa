#include <cassert>
#include <algorithm>

#include "analysis_utils.hpp"


void print_test_result(bool b, const std::string &func_name)
{
    std::cout << "Test for " << func_name << (b ? ": Passed!" : ": Failed!") << std::endl;
}

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


template<typename T>
bool are_sets_equal(const std::set<T> &set_1, const std::set<T> &set_2)
{
    bool set_1_in_set_2 = std::includes(set_1.begin(), set_1.end(), set_2.begin(), set_2.end());
    bool set2_in_set_1 = std::includes(set_2.begin(), set_2.end(), set_1.begin(), set_1.end());
    return set_1_in_set_2 && set2_in_set_1;
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
    print_test_result(are_sets_equal<Var>(res, expected), __func__);
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
    print_test_result(are_sets_equal<Var>(res, expected), __func__);
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
    print_test_result(are_sets_equal<Var>(res, expected), __func__);
}


void test_free_variables_stmt_complex(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    std::set<Var> res = free_variables_stmt(WLangTestPrograms::factorial_stmt);
    std::set<Var> expected = { Var("y"), Var("x"), Var("z") };
    print_test_result(are_sets_equal<Var>(res, expected), __func__);
}

void test_program_points_complex(bool print)
{
    if(print) WLangTestPrograms::factorial_stmt->print_node(0);

    std::set<PP> res = program_points(WLangTestPrograms::factorial_stmt);
    std::set<PP> expected = { PP(1), PP(2), PP(3), PP(4), PP(5), PP(6) };
    print_test_result(are_sets_equal<PP>(res, expected), __func__);
}

