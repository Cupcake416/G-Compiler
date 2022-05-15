
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
class BinaryExprNode;
class CallExprNode;
class AssignStmtNode;
class IfStmtNode;
class CompoundStmtNode;

class Node {
public: 
    virtual ~Node() {}
    virtual llvm::Value* codeGen() = 0;
    virtual std::string jsonGen() {
        return "";
    };
};

class ExprNode: public Node {

};

class StmtNode: public Node {

};

class Identifier: public ExprNode {
public:
    Identifier(std::string name): name(name) {}
    llvm::Value* codeGen() override;
    llvm::Value* addrGen() override;
    std::string jsonGen() override;
    std::string name;
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
    llvm::Value* codeGen() override;
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

// class TypeNode: public StmtNode {
// public: 
//     enum Type {
//         TYPE_INT,
//         TYPE_REAL,
//         TYPE_CHAR,
//         TYPE_BOOL
//     };
//     TypeNode(Type type): type(type) {}
//     llvm::Value* codeGen() override;
//     std::string jsonGen() override;
// private:
//     Type type;
// };
enum NodeType {
    TYPE_INT,
    TYPE_REAL,
    TYPE_CHAR,
    TYPE_BOOL
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
    CallExprNode(Identifier callee, std::vector<ExprNode*> *args): callee(callee), args(args) {}
    llvm::Value* codeGen() override;
    std::string jsonGen() override;
private:
    Identifier callee;
    std::vector<ExprNode*> *args;
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
