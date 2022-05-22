## 抽象语法树

### 抽象语法树的结构定义

#### Node类

Node类是抽象语法树中所有节点的共同祖先。Node类是一个抽象类，拥有三个虚函数，分别是codeGen，dotGen和getClass，用于生成中间代码,生成DOT语言脚本和获得类名。

```c++
class Node {
public: 
    virtual llvm::Value* codeGen() = 0;
    virtual void dotGen(std::string *dot) {
        return;
    };
    virtual std::string getClass() = 0;
};
```

#### ExprNode和StmtNode类

我们将各种实体类分为两大部分，分别继承自ExprNode和StmtNode。ExprNode类表示表达式类，它的子类表示一个值或者可被指定一个值，比如变量。StmtNode类表示语句类，该类的特征是会进行操作，如赋值，定义变量等。

```c++
class StmtNode: public Node {

};

// expr也可以看做stmt，1+1; 是合法语句
// 这样方便函数与过程的统一调用
class ExprNode: public StmtNode {

};
```

#### 标识符

用Identifier类来表示一个标识符，其中包含一个name字段和index字段。name表示变量名。当Identifier表示一个数组元素时，index字段用于表示数组下标。

```c++
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
    void dotGen(std::string *dot) override;
    std::string getClass() override {return "Identifier";}
    std::string name;
    ExprNode* index;
    int len;
};
```

#### 常量

ConstExprNode类表示一个常量。其下有四个子类IntegerExprNode、DoubleExprNode、CharExprNode和BooleanExprNode，分别表示对应类型的常量。

```c++
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
```

```c++
class IntegerExprNode: public ConstExprNode {
public:
    IntegerExprNode(int value): value(value) {}
    virtual ConstExprNode::Value getValue() override {
        Value v;
        v.i = value;
        return v;
    }
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::string getClass() override {return "IntegerExprNode";}
private:
    int value;
};
```

#### 常量声明和变量声明

ConstDeclNode表示常量声明节点，用变量名（name），常量表达式（value）和常量类型（type）组成。VariableDeclNode表示常量声明节点，与常量声明的区别是去掉了常量表达式（value），同时储存多个变量名，支持一次声明多个变量。

```c++
class ConstDeclNode: public StmtNode {
public: 
    ConstDeclNode(Identifier *id, ConstExprNode *value, NodeType type): name(id), value(value), type(type) {}
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::string getClass() override {return "ConstDeclNode";}
private:
    Identifier *name;
    ConstExprNode *value;
    NodeType type;
};
```

```c++
class VariableDeclNode: public StmtNode {
public:
    VariableDeclNode(std::vector<Identifier*> *nameList, NodeType type): nameList(nameList), type(type) {}
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::string getClass() override {return "VariableDeclNode";}
private:
    std::vector<Identifier*> *nameList;
    NodeType type;
};
```

NodeType定义如下：

```c++
enum NodeType {
    TYPE_INT,
    TYPE_REAL,
    TYPE_CHAR,
    TYPE_BOOL
};
```

#### 函数声明和调用

FuncDecNode和CallExprNode分别表示函数的声明和调用语句。

FuncDecNode包含函数名，返回值类型，参数列表和函数体：

```c++
class FuncDecNode: public StmtNode {
public:
    FuncDecNode(Identifier* name, FuncType type, std::vector<std::pair<NodeType, Identifier*> > *argList, CompoundStmtNode* body): name(name), type(type), argList(argList), body(body) {}
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::string getClass() override {return "FuncDecNode";}
private:
    Identifier* name;
    FuncType type;  // return type
    std::vector<std::pair<NodeType, Identifier*> > *argList; // set to null if no args
    CompoundStmtNode* body;
};
```

CallExprNode包含调用函数名和参数列表：

```c++
class CallExprNode: public ExprNode {
public:
    CallExprNode(Identifier* callee, std::vector<ExprNode*> *args): callee(callee), args(args) {}
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::string getClass() override {return "CallExprNode";}
private:
    Identifier* callee;
    std::vector<ExprNode*> *args;
};
```

#### 复合语句

复合语句表示一系列语句，包含一个语句列表

```c++
class CompoundStmtNode : public StmtNode {
public:
    CompoundStmtNode(std::vector<StmtNode*> *stmtList) : stmtList(stmtList) {}
    llvm::Value* codeGen() override;
    void dotGen(std::string *dot) override;
    std::string getClass() override {return "CompoundStmtNode";}
private:
    std::vector<StmtNode*> *stmtList;
};
```

#### 其他语句

上面介绍了基本的顶层类和几个重要的底层类，其他语句如二元运算符、赋值语句、控制语句、输入输出语句的实现大同小异，不作详细介绍。

### 语法分析的具体实现

### 抽象语法树的可视化

我们使用Graphviz实现抽象语法树的可视化。Graphviz是一个开源工具包，用于绘制DOT语言脚本描述的图形。首先调用Visualization类中的dotGen函数生成DOT语言的框架，然后调用每个AST节点中的dotGen函数获得完整的DOT语言脚本。

Visualization类中的dotGen函数：

```c++
std::string Visualization::dotGen(Node *root) {
    std::string dot = "";
    dot += "digraph G{\n";
    dot += "n" + std::to_string(nodeNum) + " [label=" + root->getClass() + "]\n";
    root->dotGen(&dot);
    dot += "}\n";
    return dot;
}
```

AST节点中的dotGen函数（以BinaryExprNode为例）：

```c++
void BinaryExprNode::dotGen(std::string *dot) {
    std::string fromName = getClass();
    int rootNum = nodeNum;
    if(lhs != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + lhs->getClass() + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        lhs->dotGen(dot);
    }
    if(rhs != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + rhs->getClass() + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        rhs->dotGen(dot);
    }
    return;
}
```

可视化结果如下：

