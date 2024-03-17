#pragma once

#include <string>
#include <utility>
#include <memory>
#include <iostream>


class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print_node(int indent) const = 0;
};

class Stmt: public ASTNode {};
class AExp: public ASTNode {};
class BExp: public ASTNode {};

class OpA: public AExp {
protected:
    std::unique_ptr<AExp> lhs;
    std::unique_ptr<AExp> rhs;

public:
    OpA(std::unique_ptr<AExp> lhs, std::unique_ptr<AExp> rhs):
        lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    void print_node(int indent) const override = 0;
};

class OpB: public BExp {
protected:
    std::unique_ptr<BExp> lhs;
    std::unique_ptr<BExp> rhs;

public:
    OpB(std::unique_ptr<BExp> lhs, std::unique_ptr<BExp> rhs):
        lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    void print_node(int indent) const override = 0;
};

class OpR: public BExp {
protected:
    std::unique_ptr<AExp> lhs;
    std::unique_ptr<AExp> rhs;

public:
    OpR(std::unique_ptr<AExp> lhs, std::unique_ptr<AExp> rhs):
        lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    void print_node(int indent) const override = 0;
};

class PP: public ASTNode {
    unsigned int pp;

public:
    explicit PP(unsigned int i): pp(i) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "PP(" << pp << ")" << std::endl;
    }
};

class Block {
protected:
    std::unique_ptr<PP> pp;

public:
    explicit Block(std::unique_ptr<PP> pp): pp(std::move(pp)) {}
};

class Var: public AExp {
    std::string var;

public:
    explicit Var(std::string var): var(std::move(var)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Var(" << var << ")" << std::endl;
    }
};

class Num: public AExp {
    unsigned int num;

public:
    explicit Num(unsigned int num): num(num) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Num(" << num << ")" << std::endl;
    }
};

class Bool: public BExp {
    bool b;

public:
    explicit Bool(bool b): b(b) {}

    void print_node(int indent) const override {
        std::string b_string = b ? "true" : "false";
        std::cout << std::string(indent, ' ') << "Bool(" << b_string << ")" << std::endl;
    }
};

class Add: public OpA {
public:
    using OpA::OpA; // Inherit constructors from OpA

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Add" << std::endl;
        lhs->print_node(indent + 2);
        rhs->print_node(indent + 2);
    }
};

class Sub: public OpA {
public:
    using OpA::OpA; // Inherit constructors from OpA

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Sub" << std::endl;
        lhs->print_node(indent + 2);
        rhs->print_node(indent + 2);
    }
};

class Mul: public OpA {
public:
    using OpA::OpA; // Inherit constructors from OpA

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Mul" << std::endl;
        lhs->print_node(indent + 2);
        rhs->print_node(indent + 2);
    }
};

class Not: public BExp {
    std::unique_ptr<BExp> b_exp;
public:
    explicit Not(std::unique_ptr<BExp> b_exp): b_exp(std::move(b_exp)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Not" << std::endl;
        b_exp->print_node(indent + 2);
    }
};

class Smaller: public OpR {
public:
    using OpR::OpR; // Inherit constructors from OpR

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Smaller" << std::endl;
        lhs->print_node(indent + 2);
        rhs->print_node(indent + 2);
    }
};

class SmallerOrEquals: public OpR {
public:
    using OpR::OpR; // Inherit constructors from OpR

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "SmallerOrEquals" << std::endl;
        lhs->print_node(indent + 2);
        rhs->print_node(indent + 2);
    }
};

class Greater: public OpR {
public:
    using OpR::OpR; // Inherit constructors from OpR

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Greater" << std::endl;
        lhs->print_node(indent + 2);
        rhs->print_node(indent + 2);
    }
};

class GreaterOrEquals: public OpR {
public:
    using OpR::OpR; // Inherit constructors from OpR

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "GreaterOrEquals" << std::endl;
        lhs->print_node(indent + 2);
        rhs->print_node(indent + 2);
    }
};

class And: public OpB {
public:
    using OpB::OpB; // Inherit constructors from OpB

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "And" << std::endl;
        lhs->print_node(indent + 2);
        rhs->print_node(indent + 2);
    }
};

class Or: public OpB {
public:
    using OpB::OpB; // Inherit constructors from OpB

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Or" << std::endl;
        lhs->print_node(indent + 2);
        rhs->print_node(indent + 2);
    }
};

class Skip: public Block, public Stmt {
public:
    using Block::Block; // Inherit constructors from Block

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Skip" << std::endl;
        pp->print_node(indent + 2);
    }
};

class Ass: public Block, public Stmt {
    std::unique_ptr<AExp> a_exp;
    std::unique_ptr<Var> var;

public:
    Ass(std::unique_ptr<Var> var, std::unique_ptr<AExp> a_exp, std::unique_ptr<PP> pp):
        Block(std::move(pp)), var(std::move(var)), a_exp(std::move(a_exp)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Ass" << std::endl;
        var->print_node(indent + 2);
        a_exp->print_node(indent + 2);
        pp->print_node(indent + 2);
    }
};


class Cond: public Block, public ASTNode {
    std::unique_ptr<BExp> b_exp;

public:
    Cond(std::unique_ptr<BExp> b_exp, std::unique_ptr<PP> pp):
        Block(std::move(pp)), b_exp(std::move(b_exp)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Cond" << std::endl;
        b_exp->print_node(indent + 2);
        pp->print_node(indent + 2);
    }
};

class SeqComp: public Stmt {
    std::unique_ptr<Stmt> stmt_1;
    std::unique_ptr<Stmt> stmt_2;

public:
    SeqComp(std::unique_ptr<Stmt> stmt_1, std::unique_ptr<Stmt> stmt_2):
        stmt_1(std::move(stmt_1)), stmt_2(std::move(stmt_2)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "SeqComp" << std::endl;
        stmt_1->print_node(indent + 2);
        stmt_2->print_node(indent + 2);
    }
};

class If: public Stmt {
    std::unique_ptr<Cond> cond;
    std::unique_ptr<Stmt> if_branch;
    std::unique_ptr<Stmt> else_branch;

public:
    If(std::unique_ptr<Cond> cond, std::unique_ptr<Stmt> if_branch, std::unique_ptr<Stmt> else_branch):
        cond(std::move(cond)), if_branch(std::move(if_branch)), else_branch(std::move(else_branch)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "If" << std::endl;
        cond->print_node(indent + 2);
        if_branch->print_node(indent + 2);
        else_branch->print_node(indent + 2);
    }
};

class While: public Stmt {
    std::unique_ptr<Cond> cond;
    std::unique_ptr<Stmt> body;

public:
    While(std::unique_ptr<Cond> cond, std::unique_ptr<Stmt> body):
            cond(std::move(cond)), body(std::move(body)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "While" << std::endl;
        cond->print_node(indent + 2);
        body->print_node(indent + 2);
    }
};
