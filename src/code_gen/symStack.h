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
    int size;
    // For variable / function, size = 0
    llvm::Value* addr;
    // For a constant, 'addr' stores its value because it's not allocated
    // For function params, 'addr' set to null: have to search them in llvm symbol table
}

class SymMap {
public:
    SymItem* find(std::string name);
    void add(std::string name, llvm::Type* ty, bool isArray, bool isConstant, int size, llvm::Value* addr);
private:
    std::map<std::string, SymItem*> mp;
}

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
}

void SymStack::add(std::string name, llvm::Type* ty, bool isArray, bool isConstant, int size, llvm::Value* addr)
{
    mapStack.back()->add(name, ty, isArray, isConstant, size, addr);
}

SymItem* SymStack::find(std::string name)
{
    for(int i = mapStack.size() - 1; i >= 0; i--)
    {
        SymItem* ret = mapStack[i].find(name);
        if(ret != nullptr) return ret;
    }
    return nullptr;
}

SymItem* SymStack::findCur(std::string name)
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

void SymMap::add(std::string name, llvm::Type* ty, bool isArray, bool isConstant, int size, llvm::Value* addr)
{
    mp[name] = new SymItem(ty, isArray, isConstant, size, addr);
}

SymItem* SymMap::find(std::string name)
{
    if(!mp.count(name)) return nullptr;
    return mp[name];
}