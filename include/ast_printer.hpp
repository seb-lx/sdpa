#pragma once

#include <iostream>

#include "ast.hpp"


struct ASTPrinter {

    void operator()(const Skip& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "Skip(" << stmt.pp_ << ")";
    }

    void operator()(const Assign& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "Assign(\n";
        print(*stmt.var_, depth + 1);
        std::cout << ",\n";
        print(*stmt.aexp_, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << stmt.pp_ << "\n";
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const If& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "If(\n";
        print(*stmt.cond_->bexp_, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << stmt.cond_->pp_ << ",\n";
        print(*stmt.then_, depth + 1);
        std::cout << ",\n";
        print(*stmt.else_, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const While& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "While(\n";
        print(*stmt.cond_->bexp_, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << stmt.cond_->pp_ << ",\n";
        print(*stmt.body_, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const SeqComp& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "SeqComp(\n";
        print(*stmt.fst_, depth + 1);
        std::cout << ",\n";
        print(*stmt.snd_, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const Var& var, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "Var(" << var.name_ << ")";
    }

    void operator()(const Num& num, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "Num(" << num.val_ << ")";
    }

    void operator()(const ArithmeticOp& op, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "ArithmeticOp(\n";
        print(*op.lhs_, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << op.op_ << ",\n";
        print(*op.rhs_, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const True&, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "True";
    }

    void operator()(const False&, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "False";
    }

    void operator()(const Not& not_op, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "Not(";
        print(*not_op.b_, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const BooleanOp& op, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "BooleanOp(\n";
        print(*op.lhs_, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << op.op_ << ",\n";
        print(*op.rhs_, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const RelationalOp& op, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "RelationalOp(\n";
        print(*op.lhs_, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << op.op_ << ",\n";
        print(*op.rhs_, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    template<typename T>
    void print_node(const T& node, const unsigned int depth) const {
        std::visit([this, depth](const auto& _node) {
            this->operator()(_node, depth);
        }, node);
    }

    void print(const Stmt& stmt, const unsigned int depth) const {
        print_node(stmt, depth);
    }

    void print(const AExp& aexp, const unsigned int depth) const {
        print_node(aexp, depth);
    }

    void print(const BExp& bexp, const unsigned int depth) const {
        print_node(bexp, depth);
    }

    template<typename T>
    void print_AST(const T& node) const {
        std::cout << "==================== PARSER ====================\n";
        print(node, 0);
        std::cout << "\n================================================\n\n";
    }

private:
    [[nodiscard]] std::string get_tabs_based_on_depth(const unsigned int depth) const {
        return std::string(depth, '\t');
    }
};
