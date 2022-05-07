
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ostream>

#include <llvm/IR/Value.h>

//所有表达式节点的基类
class ExprNode {
public: 
    virtual ~ExprNode() {}
    virtual llvm::Value* codeGen() = 0;
};

class NumberExprNode: public ExprNode {
public:
    NumberExprNode(double val): val(val) {}
    llvm::Value *codeGen() override;
private:
    double val;
};

class VariableExprNode: public ExprNode {
public:
    VariableExprNode(std::string name): name(name) {}
    llvm::Value *codeGen() override;
private:
    std::string name;
};

class BinaryExprNode: public ExprNode {
public:
    BinaryExprNode(char op, std::unique_ptr<ExprNode> lhs, std::unique_ptr<ExprNode> rhs): op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    llvm::Value *codeGen() override;
private:
    char op;
    std::unique_ptr<ExprNode> lhs;
    std::unique_ptr<ExprNode> rhs;
 };


class CallExprNode: public ExprNode {
public:
    CallExprNode(std::string callee, std::vector<std::unique_ptr<ExprNode>> args): callee(callee), args(std::move(args)) {}
    llvm::Value *codeGen() override;
private:
    std::string callee;
    std::vector<std::unique_ptr<ExprNode>> args;
};