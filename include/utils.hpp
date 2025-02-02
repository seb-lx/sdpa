#pragma once

#include <vector>
#include <set>
#include <unordered_set>

#include "ast.hpp"


// https://www.cppstories.com/2019/02/2lines3featuresoverload.html/
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;


// Custom comparator for Block pointer
// so that std::set orders the elements according to the underlying block instead of memory addresses
struct BlockPtrCmp {
    bool operator()(const Block* lhs, const Block* rhs) const {
        return lhs->pp_ < rhs->pp_;
    }
};
// ... same for Var pointer
struct VarPtrCmp {
    bool operator()(const Var* lhs, const Var* rhs) const {
        return lhs->name_ < rhs->name_;
    }
};


// Types for readability
using FreeVariables = std::set<const Var*, VarPtrCmp>;
using ElementaryBlocks = std::set<const Block*, BlockPtrCmp>;
using ControlFlowEdge = std::pair<PP, PP>;
using CFG = std::set<ControlFlowEdge>;
using LiveVariables = std::set<const Var*, VarPtrCmp>;
using LiveVariablesVec = std::vector<LiveVariables>;

