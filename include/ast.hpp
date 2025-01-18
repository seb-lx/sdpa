#pragma once

#include <variant>
#include <memory>


// Forward declarations
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

struct Cond;

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

// Program points
using PP = unsigned int;

// This is a base struct for all possible elementary blocks:
// Skip, Assign and Cond (if or while condition)
struct Block {
    PP pp_; 

    Block(PP pp): pp_{pp} {}

    virtual ~Block() = default;
};

struct Skip: public Block {
    Skip(PP pp): Block{pp} {}
};

struct Assign: public Block {
    std::unique_ptr<Var> var_;
    std::unique_ptr<AExp> aexp_;

    Assign(PP pp, std::unique_ptr<Var> var, std::unique_ptr<AExp> aexp): 
        Block{pp}, var_{std::move(var)}, aexp_{std::move(aexp)} {}
};

struct Cond: public Block {
    std::unique_ptr<BExp> bexp_;

    Cond(PP pp, std::unique_ptr<BExp> bexp): 
        Block{pp}, bexp_{std::move(bexp)} {}
};

struct If {
    std::unique_ptr<Cond> cond_;
    std::unique_ptr<Stmt> then_;
    std::unique_ptr<Stmt> else_;
};

struct While {
    std::unique_ptr<Cond> cond_;
    std::unique_ptr<Stmt> body_;
};

struct SeqComp {
    std::unique_ptr<Stmt> fst_;
    std::unique_ptr<Stmt> snd_;
};

struct Var {
    std::string name_;
};

struct Num {
    unsigned int val_;
};

struct ArithmeticOp {
    std::unique_ptr<AExp> lhs_;
    std::string op_;
    std::unique_ptr<AExp> rhs_;
};

struct True {};
struct False {};

struct Not {
    std::unique_ptr<BExp> b_;
};

struct BooleanOp {
    std::unique_ptr<BExp> lhs_;
    std::string op_;
    std::unique_ptr<BExp> rhs_;
};

struct RelationalOp {
    std::unique_ptr<AExp> lhs_;
    std::string op_;
    std::unique_ptr<AExp> rhs_;
};
