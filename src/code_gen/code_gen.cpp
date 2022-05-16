#include "code_gen.h"

Generator *generator;

llvm::AllocaInst* getAlloc(llvm::BasicBlock *BB, llvm::StringRef VarName, llvm::Type* type) {
  llvm::IRBuilder<> tmp(BB);
  return tmp.CreateAlloca(type, nullptr, VarName);
}

llvm::Value* exitError(const std::string s)
{
    fprintf(stderr, "%s\n", s.c_str());
    return nullptr;
}

llvm::Function* Generator::getCurFunction()
{
    return funcStack.top();
}

void Generator::pushFunction(llvm::Function* func)
{
    funcStack.push(func);
}

void Generator::popFunction()
{
    funcStack.pop();
}

void Generator::generate(Node* root)
{
    root->codeGen();
    this->module->print(llvm::outs(), nullptr);
}

Generator::Generator(/* args */)
{
    this->module = new llvm::Module("main", context);
    
    std::vector<llvm::Type*> arg_types;
    arg_types.push_back(builder.getInt8PtrTy());
    auto printf_type = llvm::FunctionType::get(builder.getInt32Ty(), llvm::makeArrayRef(arg_types), true);
    auto func = llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, llvm::Twine("printf"), this->module);
    func->setCallingConv(llvm::CallingConv::C);
    this->printf = func;

    auto scanf_type = llvm::FunctionType::get(builder.getInt32Ty(), true);
    func = llvm::Function::Create(scanf_type, llvm::Function::ExternalLinkage, llvm::Twine("scanf"), this->module);
    func->setCallingConv(llvm::CallingConv::C);
    this->scanf = func;
    
    this->symStack = new SymStack();
    // symStack.create(); // global variable namespace
}

Generator::~Generator()
{
    delete this->module;
}

llvm::Value* Identifier::codeGen()
{
    SymItem* res = generator->symStack->find(this->name);
    if(res == nullptr)
        return exitError("Undefined Variable");
    if(res->addr == nullptr)
        res->addr = generator->funcStack.top()->getValueSymbolTable()->lookup(this->name);
    if(res->isConstant)
        return res->addr;
    if(res->isArray)
    {
        llvm::Value* val = index->codeGen();
        std::vector<llvm::Value*> indexVec;
        indexVec.push_back(builder.getInt32(0));
        indexVec.push_back(val);
        llvm::Value* varPtr = builder.CreateInBoundsGEP(res->ty, res->addr, llvm::ArrayRef<llvm::Value*>(indexVec));
        return builder.CreateLoad(res->ty, varPtr);
    }
    return builder.CreateLoad(res->ty, res->addr);
}

llvm::Value* Identifier::addrGen()
{
    SymItem* res = generator->symStack->find(this->name);
    if(res == nullptr)
        return exitError("Undefined Variable");
    if(res->addr == nullptr)
        res->addr = generator->funcStack.top()->getValueSymbolTable()->lookup(this->name);
    if(res->isConstant)
        return exitError("Constants can't be assigned");
    if(res->isArray)
    {
        llvm::Value* val = index->codeGen();
        std::vector<llvm::Value*> indexVec;
        indexVec.push_back(builder.getInt32(0));
        indexVec.push_back(val);
        return builder.CreateInBoundsGEP(res->ty, res->addr, llvm::ArrayRef<llvm::Value*>(indexVec));
    }
    return res->addr;
}

llvm::Value* IntegerExprNode::codeGen()
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, value));
}

llvm::Value* DoubleExprNode::codeGen() {
    return llvm::ConstantFP::get(context, llvm::APFloat(value));
}

llvm::Value* CharExprNode::codeGen()
{
    return llvm::ConstantInt::get(context, llvm::APInt(8, value));
}

llvm::Value* BooleanExprNode::codeGen()
{
    return llvm::ConstantInt::get(context, llvm::APInt(8, value ? 1 : 0));
}

llvm::Value* ReturnNode::codeGen()
{
    if(res != nullptr)
        return builder.CreateRet(res->codeGen());
    return builder.CreateRetVoid();
}

llvm::Value* ConstDeclNode::codeGen()
{
    if(name->len != -1)
        return exitError("Const Array is not supported");
    if(generator->symStack->findCur(name->name))
        return exitError("Variable Redefinition");
    llvm::Type* ty;
    switch(type)
    {
    case TYPE_INT:
        ty = llvm::Type::getInt32Ty(context);
        generator->symStack->add(name->name, ty, 0, 1, 0, llvm::ConstantInt::get(context, llvm::APInt(32, value->getValue().i)));
        break;
    case TYPE_REAL:
        ty = llvm::Type::getFloatTy(context);
        generator->symStack->add(name->name, ty, 0, 1, 0, llvm::ConstantFP::get(context, llvm::APFloat(value->getValue().d)));
        break;
    case TYPE_CHAR:
        ty = llvm::Type::getInt8Ty(context);
        generator->symStack->add(name->name, ty, 0, 1, 0, llvm::ConstantInt::get(context, llvm::APInt(8, value->getValue().c)));
        break;
    default:
        ty = llvm::Type::getInt8Ty(context);
        generator->symStack->add(name->name, ty, 0, 1, 0, llvm::ConstantInt::get(context, llvm::APInt(8, value->getValue().b ? 1 : 0)));
        break;
    }
    return nullptr;
}

llvm::Value* VariableDeclNode::codeGen()
{
    llvm::Type* ty;
    switch(type)
    {
    case TYPE_INT:
        ty = llvm::Type::getInt32Ty(context);
        break;
    case TYPE_REAL:
        ty = llvm::Type::getFloatTy(context);
        break;
    default:
        ty = llvm::Type::getInt8Ty(context);
        break;
    }
    for(int i = 0; i < nameList->size(); i++)
    {
        Identifier* id = (*nameList)[i];
        if(generator->symStack->findCur(id->name))
            return exitError("Variable Redefinition");
        generator->symStack->add(id->name, ty, 0, 0, id->len == -1 ? 0 : id->len,
            getAlloc(&(generator->getCurFunction()->getEntryBlock()), id->name, id->len == -1 ? ty : llvm::ArrayType::get(ty, id->len)));
    }
    return nullptr;
}

llvm::Value* FuncDecNode::codeGen()
{
    std::vector<llvm::Type*> argTypeVec;
    std::vector<std::string> argNameVec;
    if (argList != nullptr) {
        for (int i = 0; i < argList->size(); i++) {
            bool isArr = (*argList)[i].second->len == 0;
            llvm::Type* ty;
            switch((*argList)[i].first)
            {
            case TYPE_INT:
                if(isArr) ty = llvm::Type::getInt32PtrTy(context); else ty = llvm::Type::getInt32Ty(context);
                break;
            case TYPE_REAL:
                if(isArr) ty = llvm::Type::getFloatPtrTy(context); else ty = llvm::Type::getFloatTy(context);
                break;
            default:
                if(isArr) ty = llvm::Type::getInt8PtrTy(context); else ty = llvm::Type::getInt8Ty(context);
                break;
            }
            argTypeVec.push_back(ty);
            argNameVec.push_back((*argList)[i].second->name);
        }
    }
    llvm::Type* retType = type == FUNC_INT ? llvm::Type::getInt32Ty(context) :
                          type == FUNC_REAL ? llvm::Type::getFloatTy(context) :
                          type == FUNC_VOID ? llvm::Type::getVoidTy(context) : llvm::Type::getInt8Ty(context);
    llvm::FunctionType* funcType = llvm::FunctionType::get(retType, argTypeVec, false);
    llvm::Function* func = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, name->name, generator->module);
    generator->pushFunction(func);
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "FuncEntry", func);
    builder.SetInsertPoint(BB);
    if(argList != nullptr) {  
        int i = 0;
        for (auto &Arg : func->args()) {
            Arg.setName(argNameVec[i++]);
        }
    }
    body->codeGen();
    builder.CreateRetVoid();  // additional return
    generator->popFunction();
    return func;
}

llvm::Value* BinaryExprNode::codeGen()
{
    llvm::Value* l = lhs->codeGen();
    llvm::Value* r = rhs->codeGen();
    if(!l || !r) return nullptr;
    bool isReal = l->getType() == llvm::Type::getFloatTy(context);
    llvm::Value* ret = nullptr;
    switch (op)
    {
    case OP_PLUS:
        ret = isReal ? builder.CreateFAdd(l, r) : builder.CreateAdd(l, r);
        break;
    case OP_MINUS:
        ret = isReal ? builder.CreateFSub(l, r) : builder.CreateSub(l, r);
        break;
    case OP_MUL:
        ret = isReal ? builder.CreateFMul(l, r) : builder.CreateMul(l, r);
        break;
    case OP_DIV:
        ret = isReal ? builder.CreateFDiv(l, r) : builder.CreateSDiv(l, r);
        break;
    case OP_MOD:
        ret = isReal ? builder.CreateFRem(l, r) : builder.CreateSRem(l, r);
        break;
    case OP_GE:
        ret = isReal ? builder.CreateFCmpOGE(l, r) : builder.CreateICmpSGE(l, r);
        break;
    case OP_GT:
        ret = isReal ? builder.CreateFCmpOGT(l, r) : builder.CreateICmpSGT(l, r);
        break;
    case OP_LT:
        ret = isReal ? builder.CreateFCmpOLT(l, r) : builder.CreateICmpSLT(l, r);
        break;
    case OP_LE:
        ret = isReal ? builder.CreateFCmpOLE(l, r) : builder.CreateICmpSLE(l, r);
        break;
    case OP_EQUAL:
        ret = isReal ? builder.CreateFCmpOEQ(l, r) : builder.CreateICmpEQ(l, r);
        break;
    case OP_UNEQUAL:
        ret = isReal ? builder.CreateFCmpONE(l, r) : builder.CreateICmpNE(l, r);
        break;
    case OP_AND:
        l->getType() != llvm::Type::getInt8Ty(context) || r->getType() != llvm::Type::getInt8Ty(context) ? nullptr : builder.CreateAnd(l, r);
        break;
    case OP_OR:
        l->getType() != llvm::Type::getInt8Ty(context) || r->getType() != llvm::Type::getInt8Ty(context) ? nullptr : builder.CreateOr(l, r);
        break;
    default:
        break;
    }
    return ret;
}

llvm::Value* CallExprNode::codeGen()
{
    llvm::Function* func = generator->module->getFunction(callee.name);
    if(func == nullptr)
        return exitError("Function not defined");
    if (func->arg_size() != args->size())
        return exitError("Arguments mismatch");

    std::vector<llvm::Value *> argsV;
    for (int i = 0; i < args->size(); i++)
    {
        argsV.push_back((*args)[i]->codeGen());
        if(!argsV.back()) return nullptr;
    }
    return builder.CreateCall(func, argsV);
}

llvm::Value* AssignStmtNode::codeGen()
{
    llvm::Value* l = lhs->addrGen();
    llvm::Value* r = rhs->codeGen();
    return builder.CreateStore(r, l);
}

llvm::Value* IfStmtNode::codeGen()
{
    llvm::Value* condV = condition->codeGen();
    llvm::Function* func = generator->getCurFunction();
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", func);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(context, "else", func);
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "merge", func);
    auto br = builder.CreateCondBr(condV, thenBB, elseBB);
    builder.SetInsertPoint(thenBB);
    llvm::Value* thenV = thenStmt->codeGen();
    builder.CreateBr(mergeBB);
    thenBB = builder.GetInsertBlock();
    builder.SetInsertPoint(elseBB);
    llvm::Value* elseV = elseStmt->codeGen();
    builder.CreateBr(mergeBB);
    elseBB = builder.GetInsertBlock();
    builder.SetInsertPoint(mergeBB);    
    return br;
}

llvm::Value* CompoundStmtNode::codeGen()
{
    generator->symStack->create();
    for(int i = 0; i < stmtList->size(); i++)
    {
        (*stmtList)[i]->codeGen();
    }
    generator->symStack->remove();
    return nullptr;
}