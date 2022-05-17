#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ostream>

#include <llvm/IR/Value.h>

class Node;
class ExprNode;
class StmtNode;
class Identifier;
class ConstExprNode;
class IntegerExprNode;
class DoubleExprNode;
class CharExprNode;
class BooleanExprNode;
class ConstDeclNode;
// class TypeNode;
class VariableDeclNode;
class FuncDecNode;
class BinaryExprNode;
class CallExprNode;
class AssignStmtNode;
class IfStmtNode;
class ReturnNode;
class CompoundStmtNode;

class Node {
public: 
    // virtual ~Node() {}
    virtual llvm::Value* codeGen() = 0;
    virtual std::string jsonGen() {
        return "";
    };
};

class StmtNode: public Node {

};

// expr也可以看做stmt，1+1; 是合法语句
// 这样方便函数与过程的统一调用
class ExprNode: public StmtNode {

};

//  For variable/function, only 'name' needed
//  For arrays,
// 解析表达式时，下标由index输入，如 A[i] = 0;
// 解析定义时，长度由len输入，如 int A[10];
// 作为函数参数定义时，请将len设为0，如 void f(int A[]) {...}
class Identifier: public ExprNode {
public:
    Identifier(std::string name, ExprNode* index = nullptr, int len = -1): name(name), index(index), len(len) {}
    llvm::Value* codeGen() override;
    llvm::Value* addrGen();
    std::string jsonGen() override;
    std::string name;
    ExprNode* index;
    int len;
};

class ConstExprNode: public ExprNode {
public:
    union Value {
        int i;
        double d;
        char c;
        bool b;
    };
    virtual Value getValue() = 0;
    std::string jsonGen() override;
};

class IntegerExprNode: public ConstExprNode {
public:
    IntegerExprNode(int value): value(value) {}
    virtual ConstExprNode::Value getValue() override {
        Value v;
        v.i = value;
        return v;
    }
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    int value;
};

class DoubleExprNode: public ConstExprNode {
public:
    DoubleExprNode(double value): value(value) {}
    virtual ConstExprNode::Value getValue() override {
        Value v;
        v.d = value;
        return v;
    }
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    double value;
};

class CharExprNode: public ConstExprNode {
public:
    CharExprNode(char value): value(value) {}
    virtual ConstExprNode::Value getValue() override {
        Value v;
        v.c = value;
        return v;
    }
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    char value;
};

class BooleanExprNode: public ConstExprNode {
public:
    BooleanExprNode(bool value): value(value) {}
    virtual ConstExprNode::Value getValue() override {
        Value v;
        v.b = value;
        return v;
    }
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    bool value;
};

// only used as constants in 'print' like print("Value: ", n);
class StringNode: public ExprNode {
public: 
    StringNode(std::string str): str(str) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    std::string str;
};

enum NodeType {
    TYPE_INT,
    TYPE_REAL,
    TYPE_CHAR,
    TYPE_BOOL
};

enum FuncType {
    FUNC_INT,
    FUNC_REAL,
    FUNC_CHAR,
    FUNC_BOOL,
    FUNC_VOID
};

class ConstDeclNode: public StmtNode {
public: 
    ConstDeclNode(Identifier *id, ConstExprNode *value, NodeType type): name(id), value(value), type(type) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    Identifier *name;
    ConstExprNode *value;
    NodeType type;
};

class VariableDeclNode: public StmtNode {
public:
    VariableDeclNode(std::vector<Identifier*> *nameList, NodeType type): nameList(nameList), type(type) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    std::vector<Identifier*> *nameList;
    NodeType type;
};

class FuncDecNode: public StmtNode {
public:
    FuncDecNode(Identifier* name, FuncType type, std::vector<std::pair<NodeType, Identifier*> > *argList, CompoundStmtNode* body): name(name), type(type), argList(argList), body(body) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    Identifier* name;
    FuncType type;  // return type
    std::vector<std::pair<NodeType, Identifier*> > *argList; // set to null if no args
    CompoundStmtNode* body;
};

class BinaryExprNode: public ExprNode {
public: 
    enum BinaryOperator {
        OP_PLUS,
        OP_MINUS,
        OP_MUL,
        OP_DIV,
        OP_GE,
        OP_GT, 
        OP_LT,
        OP_LE,
        OP_EQUAL,
        OP_UNEQUAL,
        OP_OR,
        OP_MOD,
        OP_AND,
    };
    BinaryExprNode(BinaryOperator op, ExprNode* lhs, ExprNode* rhs): op(op), lhs(lhs), rhs(rhs) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    std::vector<std::string> opName{"+", "-", "*", "/", ">=", ">", "<", "<=", "==", "!=", "||", "%", "&&"};
    BinaryOperator op;
    ExprNode *lhs;
    ExprNode *rhs;
};

class CallExprNode: public ExprNode {
public:
    CallExprNode(Identifier* callee, std::vector<ExprNode*> *args): callee(callee), args(args) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    Identifier* callee;
    std::vector<ExprNode*> *args;
};

// scan单独做一类，因为参数必须是变量/字符数组
// print按照CallExprNode调用即可
class ScanNode: public StmtNode {
public:
    ScanNode(std::vector<Identifier*> *args): args(args) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    std::vector<Identifier*> *args;
};

class ReturnNode: public StmtNode {
public:
    ReturnNode(ExprNode* res): res(res) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    ExprNode* res;
};

class AssignStmtNode: public StmtNode {
public:
    AssignStmtNode(Identifier *lhs, ExprNode *rhs): lhs(lhs), rhs(rhs) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    Identifier *lhs;
    ExprNode *rhs;
};

class IfStmtNode: public StmtNode {
public:
    IfStmtNode(ExprNode *condition, StmtNode *thenStmt, StmtNode *elseStmt): condition(condition), thenStmt(thenStmt), elseStmt(elseStmt) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    ExprNode *condition;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
};

class CompoundStmtNode : public StmtNode {
public:
    CompoundStmtNode(std::vector<StmtNode*> *stmtList) : stmtList(stmtList) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    std::vector<StmtNode*> *stmtList;
};
