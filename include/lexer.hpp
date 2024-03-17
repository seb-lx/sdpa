#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <iostream>

#include "utils.hpp"



class Lexer {
    std::istringstream iss;

public:
    [[nodiscard]] static std::vector<Token> tokenize(const std::string &program);
};
