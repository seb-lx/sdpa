#pragma once

#include <iostream>
#include <exception>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <filesystem>

//std::string program = "[y := 1]^1";
//std::string program = "[y := x]^1; [z := 1]^2; [z := 1]^2";
//std::string program = "while [((false and false) and false)]^31 do [z := (z * y)]^4; [y := (y - 12)]^5 od";
//std::string program = "[y := x]^1; [z := 1]^2; while [(y > 1)]^3 do [z := (z * y)]^4; [y := (y - 1)]^5 od; [y := 0]^6";
//std::string program = "[yyyYYY := x]^1; [z := 1]^2; if [(y >= 1)]^3 then [zASZ := (z * y)]^4; [y := (y - 1)]^5 else [y := 0]^6 fi; [y := 0]^7";

class WLangReader {
    std::ifstream file;

public:
    explicit WLangReader(const std::filesystem::path& file_path) {
        file.open(file_path);
        if (!file.is_open()) throw std::runtime_error("Error while opening file!");
    }
    ~WLangReader() { if(file.is_open()) file.close(); }

    [[nodiscard]] std::string read_file() const {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    };
};


enum class TokenType {
    KEYWORD,
    BOOLEAN,
    NUMBER,
    VARIABLE,
    ARITHMETIC_OP,
    BOOLEAN_OP,
    RELATIONAL_OP,
    ASSIGN_OP,
    SEMICOLON,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    SUPERSCRIPT,
    None
};

using Token = std::pair<TokenType, std::string>;


class SyntaxError: public std::exception {
private:
    std::string msg;
public:
    explicit SyntaxError(std::string msg): msg(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return msg.c_str();
    }
};
