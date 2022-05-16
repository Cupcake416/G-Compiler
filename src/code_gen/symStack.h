#pragma once

#include <cstdlib>
#include "../ast/ast.h"

class SymItem {
public:
    SymItem(llvm::Type* ty, bool isArray, bool isConstant, int size, llvm::Value* addr): ty(ty), isArray(isArray), isConstant(isConstant), size(size), addr(addr) {}
    llvm::Type* ty;
    bool isArray;
    bool isConstant;
    int size;
    // For variable / function, size = 0
    llvm::Value* addr;
    // For a constant, 'addr' stores its value because it's not allocated
    // For function params, 'addr' set to null: have to search them in llvm symbol table
};

class SymMap {
public:
    SymItem* find(std::string name);
    void add(std::string name, llvm::Type* ty, bool isArray, bool isConstant, int size, llvm::Value* addr);
private:
    std::map<std::string, SymItem*> mp;
};

class SymStack {
public:
    SymItem* find(std::string name);
    SymItem* findCur(std::string name);
    void add(std::string name, llvm::Type* ty, bool isArray, bool isConstant, int size, llvm::Value* addr);
    void create();
    void remove();
    ~SymStack();
private:
    std::vector<SymMap*> mapStack;
};