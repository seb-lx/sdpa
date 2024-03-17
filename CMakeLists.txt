cmake_minimum_required(VERSION 3.27)
project(sdpa)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(
        sdpa
        main.cpp
        src/parser.cpp
        include/parser.hpp
        src/lexer.cpp
        include/lexer.hpp
        include/ast.hpp
        include/utils.hpp
)
