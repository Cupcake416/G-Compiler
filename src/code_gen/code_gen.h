#ifndef _CODE_GEN_H_
#define _CODE_GEN_H_

#include <iostream>
#include <stack>
#include <map>
#include <string>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Target/TargetMachine.h>
#include "../ast/ast.h"
#include "SymStack.h"

using namespace std;

static llvm::LLVMContext context;
static llvm::IRBuilder<> builder(context);

class Generator {
public:
    llvm::Module *module;
    llvm::Function *printf, *scanf;
    stack<llvm::Function*> funcStack;
    stack<std::pair<llvm::BasicBlock*, llvm::BasicBlock*> > loopStack;
    SymStack* symStack;

    llvm::Function* getCurFunction();
    void pushFunction(llvm::Function* func);
    void popFunction();
    void generate(Node *root);
    Generator();
    ~Generator();

    // llvm's every BasicBlock has exactly one 'br' or 'ret' as end
    // So creating jump instructions needs record
    // 0 - not set, 1 - ret, 2 - break, 3 - continue
    int brSet = 0;
};

extern Generator* generator;

#endif
