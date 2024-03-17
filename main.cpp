#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"


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


int main(int argc, char* argv[])
{
    if (argc < 1) { std::cerr << "No command-line arguments provided." << std::endl; return 1; }

    // Read program from file
    std::string program = read_program(argv, "test.wlang");

    // Lexer to create tokens
    std::vector<Token> tokens = Lexer::tokenize(program);

    // Parser to create AST
    Parser parser = Parser(tokens);
    std::shared_ptr<Stmt> stmt = parser.parse();
    stmt->print_node(0);

    /*
    # LV-analysis
    lv = LiveVariableAnalysis(stmt);
    lv_res = lv.compute();
    tmp = None
    */

    return 0;
}


