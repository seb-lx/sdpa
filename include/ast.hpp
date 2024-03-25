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

class Stmt: public ASTNode {
private:
    [[nodiscard]] virtual bool is_stmt_equal(const Stmt &other) const = 0;
public:
    bool operator==(const Stmt &other) const { return this->is_stmt_equal(other); }
    bool operator!=(const Stmt &other) const { return !(this->is_stmt_equal(other)); }

    void print_node(int indent) const override = 0;
};

class AExp: public ASTNode {
private:
    [[nodiscard]] virtual bool is_a_exp_equal(const AExp &other) const = 0;
public:
    bool operator==(const AExp &other) const { return this->is_a_exp_equal(other); }
    bool operator!=(const AExp &other) const { return !(this->is_a_exp_equal(other)); }

    void print_node(int indent) const override = 0;
};

class BExp: public ASTNode {
private:
    [[nodiscard]] virtual bool is_b_exp_equal(const BExp &other) const = 0;
public:
    bool operator==(const BExp &other) const { return this->is_b_exp_equal(other); }
    bool operator!=(const BExp &other) const { return !(this->is_b_exp_equal(other)); }

    void print_node(int indent) const override = 0;
};

class OpA: public AExp {
public:
    std::shared_ptr<AExp> lhs;
    std::shared_ptr<AExp> rhs;

public:
    OpA(std::shared_ptr<AExp> lhs, std::shared_ptr<AExp> rhs):
        lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    void print_node(int indent) const override = 0;
};

class OpB: public BExp {
public:
    std::shared_ptr<BExp> lhs;
    std::shared_ptr<BExp> rhs;

public:
    OpB(std::shared_ptr<BExp> lhs, std::shared_ptr<BExp> rhs):
        lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    void print_node(int indent) const override = 0;
};

class OpR: public BExp {
public:
    std::shared_ptr<AExp> lhs;
    std::shared_ptr<AExp> rhs;

public:
    OpR(std::shared_ptr<AExp> lhs, std::shared_ptr<AExp> rhs):
        lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    void print_node(int indent) const override = 0;
};

class PP: public ASTNode {
    unsigned int pp;

public:
    explicit PP(unsigned int i): pp(i) {}

    bool operator<(const PP& other) const {
        return pp < other.pp;
    }

    bool operator==(const PP& other) const {
        return pp == other.pp;
    }

    bool operator!=(const PP& other) const {
        return !(*this == other); // using prev defined operator==
    }

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "PP(" << pp << ")" << std::endl;
    }
};

class Block {
protected:
    std::shared_ptr<PP> pp;

public:
    explicit Block(std::shared_ptr<PP> pp): pp(std::move(pp)) {}

    bool operator==(const Block &other) const { return this->is_block_equal(other); }

    std::shared_ptr<PP> get_program_point() { return pp; }

private:
    [[nodiscard]] virtual bool is_block_equal(const Block &other) const = 0;
};

class Var: public AExp {
    std::string var;

public:
    explicit Var(std::string var): var(std::move(var)) {}

    bool operator<(const Var& other) const { // Needed for std::set
        return var < other.var;
    }

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Var(" << var << ")" << std::endl;
    }

private:
    [[nodiscard]] bool is_a_exp_equal(const AExp &other) const override
    {
        if (const Var* other_var = dynamic_cast<const Var*>(&other))
            return this->var == other_var->var;
        return false;
    }
};

class Num: public AExp {
    unsigned int num;

public:
    explicit Num(unsigned int num): num(num) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Num(" << num << ")" << std::endl;
    }

private:
    [[nodiscard]] bool is_a_exp_equal(const AExp &other) const override
    {
        if (const Num* other_num = dynamic_cast<const Num*>(&other))
            return this->num == other_num->num;
        return false;
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

private:
    [[nodiscard]] bool is_b_exp_equal(const BExp &other) const override
    {
        if (const Bool* other_bool = dynamic_cast<const Bool*>(&other))
            return this->b == other_bool->b;
        return false;
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

private:
    [[nodiscard]] bool is_a_exp_equal(const AExp &other) const override
    {
        if (const Add* other_add = dynamic_cast<const Add*>(&other))
            return (*(this->lhs) == *(other_add->lhs)) && (*(this->rhs) == *(other_add->rhs));
        return false;
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

private:
    [[nodiscard]] bool is_a_exp_equal(const AExp &other) const override
    {
        if (const Sub* other_sub = dynamic_cast<const Sub*>(&other))
            return (*(this->lhs) == *(other_sub->lhs)) && (*(this->rhs) == *(other_sub->rhs));
        return false;
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

private:
    [[nodiscard]] bool is_a_exp_equal(const AExp &other) const override
    {
        if (const Mul* other_mul = dynamic_cast<const Mul*>(&other)) {
            return (*(this->lhs) == *(other_mul->lhs)) && (*(this->rhs) == *(other_mul->rhs));
        }
        return false;
    }
};

class Not: public BExp {
public: //protected:
    std::shared_ptr<BExp> b_exp;
public:
    explicit Not(std::shared_ptr<BExp> b_exp): b_exp(std::move(b_exp)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Not" << std::endl;
        b_exp->print_node(indent + 2);
    }

private:
    [[nodiscard]] bool is_b_exp_equal(const BExp &other) const override
    {
        if (const Not* other_not = dynamic_cast<const Not*>(&other))
            return (*(this->b_exp) == *(other_not->b_exp));
        return false;
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

private:
    [[nodiscard]] bool is_b_exp_equal(const BExp &other) const override
    {
        if (const Smaller* other_smaller = dynamic_cast<const Smaller*>(&other))
            return (*(this->lhs) == *(other_smaller->lhs)) && (*(this->rhs) == *(other_smaller->rhs));
        return false;
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

private:
    [[nodiscard]] bool is_b_exp_equal(const BExp &other) const override
    {
        if (const SmallerOrEquals* other_smaller_or_eq = dynamic_cast<const SmallerOrEquals*>(&other))
            return (*(this->lhs) == *(other_smaller_or_eq->lhs)) && (*(this->rhs) == *(other_smaller_or_eq->rhs));
        return false;
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

private:
    [[nodiscard]] bool is_b_exp_equal(const BExp &other) const override
    {
        if (const Greater* other_greater = dynamic_cast<const Greater*>(&other))
            return (*(this->lhs) == *(other_greater->lhs)) && (*(this->rhs) == *(other_greater->rhs));
        return false;
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

private:
    [[nodiscard]] bool is_b_exp_equal(const BExp &other) const override
    {
        if (const GreaterOrEquals* other_greater_or_eq = dynamic_cast<const GreaterOrEquals*>(&other))
            return (*(this->lhs) == *(other_greater_or_eq->lhs)) && (*(this->rhs) == *(other_greater_or_eq->rhs));
        return false;
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

private:
    [[nodiscard]] bool is_b_exp_equal(const BExp &other) const override
    {
        if (const And* other_and = dynamic_cast<const And*>(&other))
            return (*(this->lhs) == *(other_and->lhs)) && (*(this->rhs) == *(other_and->rhs));
        return false;
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

private:
    [[nodiscard]] bool is_b_exp_equal(const BExp &other) const override
    {
        if (const Or* other_or = dynamic_cast<const Or*>(&other))
            return (*(this->lhs) == *(other_or->lhs)) && (*(this->rhs) == *(other_or->rhs));
        return false;
    }
};

class Skip: public Block, public Stmt {
public:
    using Block::Block; // Inherit constructors from Block

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Skip" << std::endl;
        pp->print_node(indent + 2);
    }

private:
    [[nodiscard]] bool is_stmt_equal(const Stmt &other) const override
    {
        if (const Skip* other_skip_stmt = dynamic_cast<const Skip*>(&other))
            return *pp == *(other_skip_stmt->pp);
        return false;
    }

    [[nodiscard]] bool is_block_equal(const Block &other) const override
    {
        if (const Skip* other_skip_block = dynamic_cast<const Skip*>(&other))
            return *pp == *(other_skip_block->pp);
        return false;
    }
};

class Ass: public Block, public Stmt {
public: //protected:
    std::shared_ptr<AExp> a_exp;
    std::shared_ptr<Var> var;

public:
    Ass(std::shared_ptr<Var> var, std::shared_ptr<AExp> a_exp, std::shared_ptr<PP> pp):
        Block(std::move(pp)), var(std::move(var)), a_exp(std::move(a_exp)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Ass" << std::endl;
        var->print_node(indent + 2);
        a_exp->print_node(indent + 2);
        pp->print_node(indent + 2);
    }

private:
    [[nodiscard]] bool is_stmt_equal(const Stmt &other) const override
    {
        if (const Ass* other_ass_stmt = dynamic_cast<const Ass*>(&other))
            return (*a_exp == *(other_ass_stmt->a_exp))
                && (*var == *(other_ass_stmt->var))
                && (*pp == *(other_ass_stmt->pp));
        return false;
    }

    [[nodiscard]] bool is_block_equal(const Block &other) const override
    {
        if (const Ass* other_ass_stmt = dynamic_cast<const Ass*>(&other))
            return (*a_exp == *(other_ass_stmt->a_exp))
                && (*var == *(other_ass_stmt->var))
                && (*pp == *(other_ass_stmt->pp));
        return false;
    }
};


class Cond: public Block, public ASTNode {
public: //protected:
    std::shared_ptr<BExp> b_exp;

public:
    Cond(std::shared_ptr<BExp> b_exp, std::shared_ptr<PP> pp):
        Block(std::move(pp)), b_exp(std::move(b_exp)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "Cond" << std::endl;
        b_exp->print_node(indent + 2);
        pp->print_node(indent + 2);
    }

private:
    [[nodiscard]] bool is_block_equal(const Block &other) const override
    {
        if (const Cond* other_cond_block = dynamic_cast<const Cond*>(&other))
            return (*b_exp == *(other_cond_block->b_exp)) && (*pp == *(other_cond_block->pp));
        return false;
    }
};

class SeqComp: public Stmt {
public: //protected:
    std::shared_ptr<Stmt> stmt_1;
    std::shared_ptr<Stmt> stmt_2;

public:
    SeqComp(std::shared_ptr<Stmt> stmt_1, std::shared_ptr<Stmt> stmt_2):
        stmt_1(std::move(stmt_1)), stmt_2(std::move(stmt_2)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "SeqComp" << std::endl;
        stmt_1->print_node(indent + 2);
        stmt_2->print_node(indent + 2);
    }

private:
    [[nodiscard]] bool is_stmt_equal(const Stmt &other) const override
    {
        if (const SeqComp* other_seq_comp_stmt = dynamic_cast<const SeqComp*>(&other))
            return (*stmt_1 == *(other_seq_comp_stmt->stmt_1)) && (*stmt_2 == *(other_seq_comp_stmt->stmt_2));
        return false;
    }
};

class If: public Stmt {
public: //protected:
    std::shared_ptr<Cond> cond;
    std::shared_ptr<Stmt> if_branch;
    std::shared_ptr<Stmt> else_branch;

public:
    If(std::shared_ptr<Cond> cond, std::shared_ptr<Stmt> if_branch, std::shared_ptr<Stmt> else_branch):
        cond(std::move(cond)), if_branch(std::move(if_branch)), else_branch(std::move(else_branch)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "If" << std::endl;
        cond->print_node(indent + 2);
        if_branch->print_node(indent + 2);
        else_branch->print_node(indent + 2);
    }

private:
    [[nodiscard]] bool is_stmt_equal(const Stmt &other) const override
    {
        if (const If* other_if_stmt = dynamic_cast<const If*>(&other))
            return (*cond == *(other_if_stmt->cond))
                && (*else_branch == *(other_if_stmt->else_branch))
                && (*if_branch == *(other_if_stmt->if_branch));
        return false;
    }
};

class While: public Stmt {
public: //protected:
    std::shared_ptr<Cond> cond;
    std::shared_ptr<Stmt> body;

public:
    While(std::shared_ptr<Cond> cond, std::shared_ptr<Stmt> body):
            cond(std::move(cond)), body(std::move(body)) {}

    void print_node(int indent) const override {
        std::cout << std::string(indent, ' ') << "While" << std::endl;
        cond->print_node(indent + 2);
        body->print_node(indent + 2);
    }

private:
    [[nodiscard]] bool is_stmt_equal(const Stmt &other) const override
    {
        if (const While* other_while_stmt = dynamic_cast<const While*>(&other))
            return (*cond == *(other_while_stmt->cond)) && (*body == *(other_while_stmt->body));
        return false;
    }
};
