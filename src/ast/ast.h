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
class VariableDeclNode;
class FuncDecNode;
class BinaryExprNode;
class CallExprNode;
class AssignStmtNode;
class IfStmtNode;
class ReturnNode;
class WhileStmtNode;
class CompoundStmtNode;

class Node {
public: 
    // virtual ~Node() {}
    virtual llvm::Value* codeGen() = 0;
    virtual void dotGen(std::string *dot) {
        return;
    };
    int line;
    std::string className;
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
// 由于缺少指针实现，函数参数定义只使用变量
class Identifier: public ExprNode {
public:
    Identifier(int line, std::string name, ExprNode* index = nullptr, int len = -1): name(name), index(index), len(len) {
        this->line = line;
        this->className = "Identifier";
    }
    llvm::Value* codeGen() override;
    llvm::Value* addrGen();
    void dotGen(std::string *dot) override;
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
    void dotGen(std::string *dot) override;
};

class IntegerExprNode: public ConstExprNode {
public:
    IntegerExprNode(int line, int value): value(value) {
        this->line = line;
        this->className = "IntegerExprNode";
    }
    virtual ConstExprNode::Value getValue() override {
        Value v;
        v.i = value;
        return v;
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    int value;
};

class DoubleExprNode: public ConstExprNode {
public:
    DoubleExprNode(int line, double value): value(value) {
        this->line = line;
        this->className = "DoubleExprNode";
    }
    virtual ConstExprNode::Value getValue() override {
        Value v;
        v.d = value;
        return v;
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    double value;
};

class CharExprNode: public ConstExprNode {
public:
    CharExprNode(int line, char value): value(value) {
        this->line = line;
        this->className = "CharExprNode";
    }
    virtual ConstExprNode::Value getValue() override {
        Value v;
        v.c = value;
        return v;
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    char value;
};

class BooleanExprNode: public ConstExprNode {
public:
    BooleanExprNode(int line, bool value): value(value) {
        this->line = line;
        this->className = "BooleanExprNode";
    }
    virtual ConstExprNode::Value getValue() override {
        Value v;
        v.b = value;
        return v;
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    bool value;
};

// only used as constants in 'print' like print("Value: ", n);
class StringNode: public ExprNode {
public: 
    StringNode(int line, std::string str): str(str) {
        this->line = line;
        this->className = "StringNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
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

union PrintItem
{
    ExprNode* exp;
    std::string* str;
};

class ConstDeclNode: public StmtNode {
public: 
    ConstDeclNode(int line, Identifier *id, ConstExprNode *value, NodeType type): name(id), value(value), type(type) {
        this->line = line;
        this->className = "ConstDeclNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    Identifier *name;
    ConstExprNode *value;
    NodeType type;
};

class VariableDeclNode: public StmtNode {
public:
    VariableDeclNode(int line, std::vector<Identifier*> *nameList, NodeType type): nameList(nameList), type(type) {
        this->line = line;
        this->className = "VariableDeclNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::vector<Identifier*> *nameList;
    NodeType type;
};

class FuncDecNode: public StmtNode {
public:
    FuncDecNode(int line, std::string name, FuncType type, std::vector<std::pair<NodeType, Identifier*> > *argList, CompoundStmtNode* body): name(name), type(type), argList(argList), body(body) {
        this->line = line;
        this->className = "FuncDecNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::string name;
    FuncType type;  // return type
    std::vector<std::pair<NodeType, Identifier*> > *argList; // set to null if no args
    CompoundStmtNode* body;
};

class BinaryExprNode: public ExprNode {
public: 
    BinaryExprNode(int line, BinaryOperator op, ExprNode* lhs, ExprNode* rhs): op(op), lhs(lhs), rhs(rhs) {
        this->line = line;
        this->className = "BinaryExprNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::vector<std::string> opName{"+", "-", "*", "/", ">=", ">", "<", "<=", "==", "!=", "||", "%", "&&"};
    BinaryOperator op;
    ExprNode *lhs;
    ExprNode *rhs;
};

class CallExprNode: public ExprNode {
public:
    CallExprNode(int line, std::string callee, std::vector<ExprNode*> *args): callee(callee), args(args) {
        this->line = line;
        this->className = "CallExprNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::string callee;
    std::vector<ExprNode*> *args;
};

// scan单独做一类，因为参数必须是变量/字符数组
class ScanNode: public StmtNode {
public:
    ScanNode(int line, std::vector<Identifier*> *args): args(args) {
        this->line = line;
        this->className = "ScanNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::vector<Identifier*> *args;
};

// Call无法输出字符串常量， print也单独分类
class PrintNode: public ExprNode {
public:
    PrintNode(int line, std::vector<std::pair<PrintItem, bool> > *args): args(args) {
        this->line = line;
        this->className = "PrintNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    Identifier* callee;
    // bool sets to true if PrintItem is exp
    std::vector<std::pair<PrintItem, bool> > *args;
};

class ReturnNode: public StmtNode {
public:
    ReturnNode(int line, ExprNode* res): res(res) {
        this->line = line;
        this->className = "ReturnNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    ExprNode* res;
};

class AssignStmtNode: public StmtNode {
public:
    AssignStmtNode(int line, Identifier *lhs, ExprNode *rhs): lhs(lhs), rhs(rhs) {
        this->line = line;
        this->className = "AssignStmtNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    Identifier *lhs;
    ExprNode *rhs;
};

class IfStmtNode: public StmtNode {
public:
    IfStmtNode(int line, ExprNode *condition, StmtNode *thenStmt, StmtNode *elseStmt): condition(condition), thenStmt(thenStmt), elseStmt(elseStmt) {
        this->line = line;
        this->className = "IfStmtNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    ExprNode *condition;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
};

class WhileStmtNode: public StmtNode {
public:
    WhileStmtNode(int line, ExprNode *condition, StmtNode *staments): condition(condition), staments(staments) {
        this->line = line;
        this->className = "WhileStmtNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    ExprNode *condition;
    StmtNode *staments;
};

class CompoundStmtNode : public StmtNode {
public:
    CompoundStmtNode(int line, std::vector<StmtNode*> *stmtList) : stmtList(stmtList) {
        this->line = line;
        this->className = "CompoundStmtNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::vector<StmtNode*> *stmtList;
};

class BreakNode: public StmtNode {
public:
    BreakNode(int line) {
        this->line = line;
        this->className = "BreakNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
};

class ContinueNode: public StmtNode {
public:
    ContinueNode(int line) {
        this->line = line;
        this->className = "ContinueNode";
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
};