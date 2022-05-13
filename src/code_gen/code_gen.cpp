#include "code_gen.h"

llvm::Function* codeGen::getCurFunction()
{
    return funStack.top();
}

void codeGen::pushFunction(llvm::Function* func)
{
    funStack.push(func);
}

void codeGen::popFunction()
{
    funStack.pop();
}

llvm::Value* codeGen::findValue(const std::string & name)
{
    llvm::Value * result = funStack.top()->getValueSymbolTable()->lookup(name);
    if (result != nullptr) {
        return result;
    }
    result = module->getGlobalVariable(name, true);
    if (result == nullptr) {
        cout<<"[ERROR]Undeclared variable: " << name <<endl;
    }
    return result;
}

llvm::Function* codeGen::createPrintf()
{
    std::vector<llvm::Type*> arg_types;
    arg_types.push_back(builder.getInt8PtrTy());
    auto printf_type = llvm::FunctionType::get(builder.getInt32Ty(), llvm::makeArrayRef(arg_types), true);
    auto func = llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, llvm::Twine("printf"), this->module);
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

llvm::Function* codeGen::createScanf()
{
    auto scanf_type = llvm::FunctionType::get(builder.getInt32Ty(), true);
    auto func = llvm::Function::Create(scanf_type, llvm::Function::ExternalLinkage, llvm::Twine("scanf"), this->module);
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

void codeGen::generate(Node* root)
{
    root->codeGen();
    this->module->print(llvm::outs(), nullptr);
}

codeGen::codeGen(/* args */)
{
    this->module = new llvm::Module("main", context);
    this->printf = this->createPrintf();
    this->scanf = this->createScanf();
}

codeGen::~codeGen()
{
    delete this->module;
}

llvm::Value* Identifier::codeGen()
{
    // TODO
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
    return llvm::ConstantInt::get(context, llvm::APInt(8, value));
}

llvm::Value* ConstDeclNode::codeGen()
{
    // TODO
}

llvm::Value* VariableDeclNode::codeGen()
{
    // TODO
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
        return fprintf(stderr, "Function not defined\n"), nullptr;
    if (func->arg_size() != args->size())
        return fprintf(stderr, "Arguments mismatch\n"), nullptr;

    std::vector<llvm::Value *> argsV;
    for (int i = 0; i < args->size(); i++)
    {
        argsV.push_back((*args)[i]->codeGen());
        if(!argsV.back()) return nullptr;
    }
    return builder.CreateCall(fun, argsV);
}

llvm::Value* AssignStmtNode::codeGen()
{
    llvm::Value* l = generator->findValue(lhs->name);
    // TODO: l may be array type
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
    for(int i = 0; i < stmtList->size(); i++)
    {
        (*stmtList)[i]->codeGen();
    }
    return nullptr;
}