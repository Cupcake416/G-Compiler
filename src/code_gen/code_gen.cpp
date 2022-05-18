#include "code_gen.h"

Generator *generator;

bool hasError = false;

llvm::AllocaInst* getAlloc(llvm::BasicBlock *BB, llvm::StringRef VarName, llvm::Type* type) {
  llvm::IRBuilder<> tmp(BB);
  return tmp.CreateAlloca(type, nullptr, VarName);
}

llvm::Value* exitError(const std::string s)
{
    fprintf(stderr, "%s\n", s.c_str());
    hasError = true;
    return nullptr;
}

llvm::Value* tryCast(llvm::Value* cur, llvm::Type* tar)
{
    if(cur->getType()->getTypeID() != tar->getTypeID())
        goto CASTLABEL;
    if(tar->isDoubleTy() || cur->getType()->getIntegerBitWidth() == tar->getIntegerBitWidth())
        return cur;
    CASTLABEL:
    llvm::Instruction::CastOps op;
    if(cur->getType()->isIntegerTy() && tar->isDoubleTy())
        op = llvm::Instruction::SIToFP;
    else if(cur->getType()->isDoubleTy() && tar->isIntegerTy())
        op = llvm::Instruction::FPToSI;
    else if(cur->getType()->isIntegerTy(32) && tar->isIntegerTy() || cur->getType()->isIntegerTy(8) && tar->isIntegerTy(1))
        op = llvm::Instruction::Trunc;
    else if(cur->getType()->isIntegerTy(8) && tar->isIntegerTy(32) || cur->getType()->isIntegerTy(1) && tar->isIntegerTy())
        op = llvm::Instruction::ZExt;
    else return exitError("Invalid Cast");

    return builder.CreateCast(op, cur, tar);
}

llvm::Function* Generator::getCurFunction()
{
    return funcStack.empty() ? nullptr : funcStack.top();
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
    if(hasError) return;
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
        return exitError("Undefined Variable: " + this->name);
    if(res->addr == nullptr)
        res->addr = generator->funcStack.top()->getValueSymbolTable()->lookup(this->name);
    if(res->isConstant)
        return res->addr;
    if(res->isArray)
    {
        if(index == nullptr) return res->addr;
        llvm::Value* val = index->codeGen();
        std::vector<llvm::Value*> indexVec;
        indexVec.push_back(builder.getInt32(0));
        indexVec.push_back(tryCast(val, llvm::Type::getInt32Ty(context)));
        llvm::Value* varPtr = builder.CreateInBoundsGEP(res->ty, res->addr, llvm::ArrayRef<llvm::Value*>(indexVec));
        return builder.CreateLoad(res->ty->getArrayElementType(), varPtr);
    }
    return builder.CreateLoad(res->ty, res->addr);
}

llvm::Value* Identifier::addrGen()
{
    SymItem* res = generator->symStack->find(this->name);
    if(res == nullptr)
        return exitError("Undefined Variable: " + this->name);
    if(res->addr == nullptr)
        res->addr = generator->funcStack.top()->getValueSymbolTable()->lookup(this->name);
    if(res->isConstant)
        return exitError("Constant " + this->name + " can't be assigned");
    if(res->isArray && index != nullptr)
    {
        llvm::Value* val = index->codeGen();
        std::vector<llvm::Value*> indexVec;
        indexVec.push_back(builder.getInt32(0));
        indexVec.push_back(tryCast(val, llvm::Type::getInt32Ty(context)));
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
    return llvm::ConstantInt::get(context, llvm::APInt(1, value ? 1 : 0));
}

llvm::Value* StringNode::codeGen()
{
    return llvm::ConstantDataArray::getString(context, str);
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
        return exitError("Variable Redefinition: " + name->name);
    llvm::Type* ty;
    switch(type)
    {
    case TYPE_INT:
        ty = llvm::Type::getInt32Ty(context);
        generator->symStack->add(name->name, ty, 0, 1, 0, llvm::ConstantInt::get(context, llvm::APInt(32, value->getValue().i)));
        break;
    case TYPE_REAL:
        ty = llvm::Type::getDoubleTy(context);
        generator->symStack->add(name->name, ty, 0, 1, 0, llvm::ConstantFP::get(context, llvm::APFloat(value->getValue().d)));
        break;
    case TYPE_CHAR:
        ty = llvm::Type::getInt8Ty(context);
        generator->symStack->add(name->name, ty, 0, 1, 0, llvm::ConstantInt::get(context, llvm::APInt(8, value->getValue().c)));
        break;
    default:
        ty = llvm::Type::getInt1Ty(context);
        generator->symStack->add(name->name, ty, 0, 1, 0, llvm::ConstantInt::get(context, llvm::APInt(1, value->getValue().b ? 1 : 0)));
        break;
    }
    return nullptr;
}

llvm::Value* VariableDeclNode::codeGen()
{
    llvm::Type* elmTy;
    llvm::Constant* zero;
    switch(type)
    {
    case TYPE_INT:
        elmTy = llvm::Type::getInt32Ty(context);
        zero = llvm::ConstantInt::get(context, llvm::APInt(32, 0));
        break;
    case TYPE_REAL:
        elmTy = llvm::Type::getDoubleTy(context);
        zero = llvm::ConstantFP::get(context, llvm::APFloat((double)0));
        break;
    case TYPE_CHAR:
        elmTy = llvm::Type::getInt8Ty(context);
        zero = llvm::ConstantInt::get(context, llvm::APInt(8, 0));
        break;
    default:
        elmTy = llvm::Type::getInt1Ty(context);
        zero = llvm::ConstantInt::get(context, llvm::APInt(1, 0));
        break;
    }
    for(int i = 0; i < nameList->size(); i++)
    {
        Identifier* id = (*nameList)[i];
        if(generator->symStack->findCur(id->name))
            return exitError("Variable Redefinition: " + id->name);
        llvm::Value* addr;
        llvm::Type* ty = id->len == -1 ? elmTy : llvm::ArrayType::get(elmTy, id->len);
        if(generator->getCurFunction() == nullptr)
        {
            llvm::GlobalVariable* globalPtr = new llvm::GlobalVariable(*(generator->module), ty, false, llvm::GlobalValue::PrivateLinkage, nullptr, id->name);
            if(id->len == -1) globalPtr->setInitializer(zero);
            else
            {
                std::vector<llvm::Constant*> arr_zero;
                for (int i = 0; i < id->len; i++) {
                    arr_zero.push_back(zero);
                }
                llvm::Constant* arr = llvm::ConstantArray::get(llvm::ArrayType::get(elmTy, id->len), arr_zero);
                globalPtr->setInitializer(arr);
            }
            addr = globalPtr;
        }
        else
            addr = getAlloc(&(generator->getCurFunction()->getEntryBlock()), id->name, ty);
        generator->symStack->add(id->name, ty, id->len != -1, 0, id->len == -1 ? 0 : id->len, addr);
    }
    return nullptr;
}

llvm::Value* FuncDecNode::codeGen()
{
    llvm::Function *func = generator->module->getFunction(name->name);
    if (func == nullptr && name->name != "main" || name->name == "scan" || name->name == "print")
        return exitError("Function redefinition: " + name->name); 
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
                if(isArr) ty = llvm::Type::getDoublePtrTy(context); else ty = llvm::Type::getDoubleTy(context);
                break;
            case TYPE_BOOL:
                if(isArr) ty = llvm::Type::getInt1PtrTy(context); else ty = llvm::Type::getInt1Ty(context);
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
                          type == FUNC_REAL ? llvm::Type::getDoubleTy(context) :
                          type == FUNC_VOID ? llvm::Type::getVoidTy(context) : 
                          type == FUNC_CHAR ? llvm::Type::getInt8Ty(context) : llvm::Type::getInt1Ty(context);
    llvm::FunctionType* funcType = llvm::FunctionType::get(retType, argTypeVec, false);
    func = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, name->name, generator->module);
    generator->pushFunction(func);
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "FuncEntry", func);
    builder.SetInsertPoint(BB);
    if(argList != nullptr) {  
        int i = 0;
        for (auto &Arg : func->args()) {
            Arg.setName(argNameVec[i++]);
        }
    }
    if(body != nullptr)
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
    
    if(op == OP_AND || op == OP_OR)
    {
        l = tryCast(l, llvm::Type::getInt1Ty(context));
        r = tryCast(r, llvm::Type::getInt1Ty(context));
        return op == OP_AND ? builder.CreateAnd(l, r) : builder.CreateOr(l, r);
    }
    
    bool isReal = false;
    llvm::Type* ty;
    if(l->getType()->isDoubleTy() || r->getType()->isDoubleTy())
        ty = llvm::Type::getDoubleTy(context), isReal = true;
    else if(l->getType()->isIntegerTy(32) || r->getType()->isIntegerTy(32))
        ty = llvm::Type::getInt32Ty(context);
    else if(l->getType()->isIntegerTy(8) || r->getType()->isIntegerTy(8))
        ty = llvm::Type::getInt8Ty(context);
    else ty = llvm::Type::getInt1Ty(context);
    l = tryCast(l, ty);
    r = tryCast(r, ty);

    switch (op)
    {
    case OP_PLUS:
        return isReal ? builder.CreateFAdd(l, r) : builder.CreateAdd(l, r);
    case OP_MINUS:
        return isReal ? builder.CreateFSub(l, r) : builder.CreateSub(l, r);
    case OP_MUL:
        return isReal ? builder.CreateFMul(l, r) : builder.CreateMul(l, r);
    case OP_DIV:
        return isReal ? builder.CreateFDiv(l, r) : builder.CreateSDiv(l, r);
    case OP_MOD:
        return isReal ? builder.CreateFRem(l, r) : builder.CreateSRem(l, r);
    case OP_GE:
        return isReal ? builder.CreateFCmpOGE(l, r) : builder.CreateICmpSGE(l, r);
    case OP_GT:
        return isReal ? builder.CreateFCmpOGT(l, r) : builder.CreateICmpSGT(l, r);
    case OP_LT:
        return isReal ? builder.CreateFCmpOLT(l, r) : builder.CreateICmpSLT(l, r);
    case OP_LE:
        return isReal ? builder.CreateFCmpOLE(l, r) : builder.CreateICmpSLE(l, r);
    case OP_EQUAL:
        return isReal ? builder.CreateFCmpOEQ(l, r) : builder.CreateICmpEQ(l, r);
    case OP_UNEQUAL:
        return isReal ? builder.CreateFCmpONE(l, r) : builder.CreateICmpNE(l, r);
    default:
        return exitError("Invalid binary operation");
    }
}

llvm::Value* CallExprNode::codeGen()
{
    llvm::Function* func = generator->module->getFunction(callee->name);
    if(func == nullptr)
        return exitError("Function not defined: " + callee->name);
    if (func->arg_size() != args->size())
        return exitError("Arguments mismatch: " + callee->name);

    std::vector<llvm::Value *> argsVec;
    for (int i = 0; i < args->size(); i++)
    {
        argsVec.push_back((*args)[i]->codeGen());
        if(!argsVec.back()) return nullptr;
    }
    return builder.CreateCall(func, argsVec);
}

llvm::Value* ScanNode::codeGen()
{
    std::string format = "";
    std::vector<llvm::Value *> argsVec;
    argsVec.push_back(nullptr);
    for (int i = 0; i < args->size(); i++)
    {
        llvm::Value* val = (*args)[i]->addrGen();
        if(val == nullptr) return nullptr;
        argsVec.push_back(val);
        SymItem res = *(generator->symStack->find((*args)[i]->name));
        if(res.isArray)
        {
            if((*args)[i]->index == nullptr)
            {
                if(res.ty->getArrayElementType()->isIntegerTy(8)) format += "%s";
                else return exitError("Illegal input type");
                continue;
            }
            else res.ty = res.ty->getArrayElementType();
        }
        if(res.ty->isIntegerTy(32)) format += "%d";
        else if(res.ty->isDoubleTy()) format += "%lf";
        else if(res.ty->isIntegerTy(1)) format += "%d";
        else if(res.ty->isIntegerTy(8)) format += "%c";
        else return exitError("Illegal input type");
    }
    argsVec[0] = builder.CreateGlobalStringPtr(format);
    return builder.CreateCall(generator->scanf, argsVec);
}

llvm::Value* PrintNode::codeGen()
{
    std::string format = "";
    std::vector<llvm::Value *> argsVec;
    argsVec.push_back(nullptr);
    for (int i = 0; i < args->size(); i++)
    {
        if((*args)[i].second)
        {
            llvm::Value* val = (*args)[i].first.exp->codeGen();
            if(val == nullptr) return nullptr;
            argsVec.push_back(val);
            if(val->getType()->isIntegerTy(32)) format += "%d";
            else if(val->getType()->isDoubleTy()) format += "%lf";
            else if(val->getType()->isIntegerTy(1)) format += "%d";
            else if(val->getType()->isIntegerTy(8)) format += "%c";
            else if(val->getType()->isPointerTy()) format += "%s";
            else return exitError("Illegal output type");
        }
        else format += *((*args)[i].first.str);
    }
    argsVec[0] = builder.CreateGlobalStringPtr(format);
    return builder.CreateCall(generator->printf, argsVec);
}

llvm::Value* AssignStmtNode::codeGen()
{
    llvm::Value* l = lhs->addrGen();
    llvm::Value* r = rhs->codeGen();
    if(!l || !r) return nullptr;
    llvm::Type* ty = generator->symStack->find(lhs->name)->ty;
    if(ty->isArrayTy())
    {
        if(lhs->index == nullptr)
            return exitError("Array " + lhs->name + " can't be assigned");
        else
            ty = ty->getArrayElementType();
    }
    r = tryCast(r, ty);
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
    if(stmtList != nullptr)
    for(int i = 0; i < stmtList->size(); i++)
    {
        (*stmtList)[i]->codeGen();
    }
    generator->symStack->remove();
    return nullptr;
}