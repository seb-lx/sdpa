cmake_minimum_required(VERSION 3.27)
project(sdpa)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(
        sdpa
        main.cpp

        include/parser.hpp
        include/lexer.hpp
        include/ast.hpp
        include/utils.hpp
        include/analysis_utils.hpp
        include/set_utils.hpp
        include/dfa/lv.hpp

        src/parser.cpp
        src/lexer.cpp
        src/analysis_utils.cpp
        src/dfa/lv.cpp

        test/test_analysis_utils.hpp
        test/test_analysis_utils.cpp
)
