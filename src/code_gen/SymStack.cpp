#include "SymStack.h"

void SymStack::add(std::string name, llvm::Type* ty, bool isArray, bool isConstant, int size, llvm::Value* addr)
{
    mapStack.back()->add(name, ty, isArray, isConstant, size, addr);
}

SymItem* SymStack::find(std::string name)
{
    for(int i = mapStack.size() - 1; i >= 0; i--)
    {
        SymItem* ret = mapStack[i]->find(name);
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