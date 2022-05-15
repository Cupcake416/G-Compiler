#pragma once

#include <vector.h>
#include <map.h>
#include <cstdlib>
#include "../ast/ast.h"

class SymItem {
public:
    llvm::Type* ty;
    bool isArray;
    bool isConstant;
    llvm::Value* addr;
    // For a constant, 'addr' stores its value because it's not allocated
}

class SymMap {
public:
    SymItem* find(Identifier name);
    void add(Identifier name, llvm::Type* ty, bool isArray, bool isConstant, llvm::Value* addr);
private:
    std::map<Identifier, SymItem*> mp;
}

class SymStack {
public:
    SymItem* find(Identifier name);
    SymItem* findCur(Identifier name);
    void add(Identifier name, llvm::Type* ty, bool isArray, bool isConstant, llvm::Value* addr);
    void create();
    void remove();
    ~SymStack();
private:
    std::vector<SymMap*> mapStack;
}

void SymStack::add(Identifier name, llvm::Type* ty, bool isArray, bool isConstant, llvm::Value* addr)
{
    mapStack.back()->add(name, ty, isArray, isConstant, addr);
}

SymItem* SymStack::find(Identifier name)
{
    for(int i = mapStack.size() - 1; i >= 0; i--)
    {
        SymItem* ret = mapStack[i].find(name);
        if(ret != nullptr) return ret;
    }
    return nullptr;
}

SymItem* SymStack::findCur(Identifier name)
{
    return mapStack.back()->find(name);
}

void SymStack::create()
{
    mapStack.push_back(new SymMap());
}

void SymStack::remove()
{
    mapStack.pop_back();
}

SymStack::~SymStack()
{
     for(int i = mapStack.size() - 1; i >= 0; i--)
        delete mapStack[i];
}

void SymMap::add(Identifier name, llvm::Type* ty, bool isArray, bool isConstant, llvm::Value* addr)
{
    mp[name] = new SymItem(ty, isArray, isConstant, addr);
}

SymItem* SymMap::find(Identifier name)
{
    if(!mp.count(name)) return nullptr;
    return mp[name];
}