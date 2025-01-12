#pragma once

#include <fstream>
#include <sstream>
#include <filesystem>
#include <memory>

class WLangReader {
private:
    std::ifstream file_;

public:
    explicit WLangReader(char* argv[], const std::string& filename) {
        if (!filename.ends_with(".wlang")) throw std::runtime_error("Wrong file type!");

        std::filesystem::path exe_path = canonical(std::filesystem::path(argv[0]));
        std::filesystem::path file_path = exe_path.parent_path().parent_path() / filename;

        file_.open(file_path);
    }

    ~WLangReader() {
        if(file_.is_open()) {
            file_.close();
        }
    }

    [[nodiscard]] std::string read_program() const {
        std::string program;

        try {
            program = read_file();
        } catch(const std::exception &e) { return ""; }

        return program;
    }

private:
    [[nodiscard]] std::string read_file() const {
        if (!file_.is_open()) throw std::runtime_error("Error while opening file!");

        std::stringstream buffer;
        buffer << file_.rdbuf();
        return buffer.str();
    }
};

