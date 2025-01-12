#pragma once

#include <variant>
#include <memory>
#include <iostream>


using PP = unsigned int;

struct Skip;
struct Assign;
struct If;
struct While;
struct SeqComp;

struct Var;
struct Num;
struct ArithmeticOp;

struct True;
struct False;
struct Not;
struct BooleanOp;
struct RelationalOp;

using Stmt = std::variant<
    Skip,
    Assign,
    If,
    While,
    SeqComp
>;

using AExp = std::variant<
    Var,
    Num,
    ArithmeticOp
>;

using BExp = std::variant<
    True,
    False,
    Not,
    BooleanOp,
    RelationalOp
>;

struct Skip {
    PP _pp;
};

struct Assign {
    std::unique_ptr<Var> _var;
    std::unique_ptr<AExp> _aexp;
    PP _pp;
};

struct If {
    std::unique_ptr<BExp> _cond;
    PP _pp;
    std::unique_ptr<Stmt> _then;
    std::unique_ptr<Stmt> _else;
};

struct While {
    std::unique_ptr<BExp> _cond;
    PP _pp;
    std::unique_ptr<Stmt> _body;
};

struct SeqComp {
    std::unique_ptr<Stmt> _fst;
    std::unique_ptr<Stmt> _snd;
};

struct Var {
    std::string _name;

    // Needed for std::set<T>
    bool operator<(const Var& other) const {
        return _name < other._name;
    }
};

struct Num {
    unsigned int _val;
};

struct ArithmeticOp {
    std::unique_ptr<AExp> _lhs;
    std::string _op;
    std::unique_ptr<AExp> _rhs;
};

struct True {};
struct False {};

struct Not {
    std::unique_ptr<BExp> _b;
};

struct BooleanOp {
    std::unique_ptr<BExp> _lhs;
    std::string _op;
    std::unique_ptr<BExp> _rhs;
};

struct RelationalOp {
    std::unique_ptr<AExp> _lhs;
    std::string _op;
    std::unique_ptr<AExp> _rhs;
};


struct ASTPrinter {
    static unsigned int depth;

    void operator()(const Skip& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "Skip(" << stmt._pp << ")";
    }

    void operator()(const Assign& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "Assign(\n";
        print(*stmt._var, depth + 1);
        std::cout << ",\n";
        print(*stmt._aexp, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << stmt._pp << "\n";
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const If& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "If(\n";
        print(*stmt._cond, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << stmt._pp << ",\n";
        print(*stmt._then, depth + 1);
        std::cout << ",\n";
        print(*stmt._else, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const While& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "While(\n";
        print(*stmt._cond, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << stmt._pp << ",\n";
        print(*stmt._body, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const SeqComp& stmt, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "SeqComp(\n";
        print(*stmt._fst, depth + 1);
        std::cout << ",\n";
        print(*stmt._snd, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const Var& var, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "Var(" << var._name << ")";
    }

    void operator()(const Num& num, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "Num(" << num._val << ")";
    }

    void operator()(const ArithmeticOp& op, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "ArithmeticOp(\n";
        print(*op._lhs, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << op._op << ",\n";
        print(*op._rhs, depth + 1);
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
        print(*not_op._b, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const BooleanOp& op, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "BooleanOp(\n";
        print(*op._lhs, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << op._op << ",\n";
        print(*op._rhs, depth + 1);
        std::cout << "\n";
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << ")";
    }

    void operator()(const RelationalOp& op, const unsigned int depth) const {
        std::cout << get_tabs_based_on_depth(depth);
        std::cout << "RelationalOp(\n";
        print(*op._lhs, depth + 1);
        std::cout << ",\n";
        std::cout << get_tabs_based_on_depth(depth + 1);
        std::cout << op._op << ",\n";
        print(*op._rhs, depth + 1);
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