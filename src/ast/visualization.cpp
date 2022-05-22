#include "visualization.h"
#include "ast.h"
#include <stdio.h>
#include <string>

int nodeNum = 0;

void Visualization::dotGen(Node *root, std::string* dot) {
    *dot += "digraph G{\n";
    *dot += "n" + std::to_string(nodeNum) + " [label=" + root->className + "]\n";
    root->dotGen(dot);
    *dot += "}\n";
    return;
}

void Identifier::dotGen(std::string *dot) {
    int rootNum = nodeNum;
    bool isArray = index != nullptr;
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=\"" + name + (isArray ? "[]" : "") +"\"]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    if(isArray) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + index->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        index->dotGen(dot);
    }
    return;
}

void ConstExprNode::dotGen(std::string *dot) {
    return;
}

void IntegerExprNode::dotGen(std::string *dot) {
    int rootNum = nodeNum;
    nodeNum += 1;
    char valueString[100];
    sprintf(valueString, "%d", getValue().i);
    *dot += "n" + std::to_string(nodeNum) + " [label=" + valueString + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    return;
}

void DoubleExprNode::dotGen(std::string *dot) {
    int rootNum = nodeNum;
    nodeNum += 1;
    char valueString[100];
    sprintf(valueString, "%.4lf", getValue().d);
    *dot += "n" + std::to_string(nodeNum) + " [label=" + valueString + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    return;
}

void CharExprNode::dotGen(std::string *dot) {
    int rootNum = nodeNum;
    nodeNum += 1;
    char valueString[100];
    sprintf(valueString, "%c", getValue().c);
    *dot += "n" + std::to_string(nodeNum) + " [label=\"" + valueString + "\"]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    return;
}

void BooleanExprNode::dotGen(std::string *dot) {
    int rootNum = nodeNum;
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + (getValue().b ? "true" : "false") + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    return;
}

void StringNode::dotGen(std::string *dot) {
    return;
}

void ConstDeclNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + name->className + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    name->dotGen(dot);
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + value->className + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    switch (type) {
    case NodeType::TYPE_INT:
        ((IntegerExprNode*)value)->dotGen(dot);
        break;
    case NodeType::TYPE_REAL:
        ((DoubleExprNode*)value)->dotGen(dot);
        break;
    case NodeType::TYPE_CHAR:
        ((CharExprNode*)value)->dotGen(dot);
        break;
    case NodeType::TYPE_BOOL:
        ((BooleanExprNode*)value)->dotGen(dot);
        break;
    }
    return;
}

void VariableDeclNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    std::string typeString;
    switch (type) {
    case NodeType::TYPE_INT:
        typeString = "Integer";
        break;
    case NodeType::TYPE_REAL:
        typeString = "Double";
        break;
    case NodeType::TYPE_CHAR:
        typeString = "Char";
        break;
    case NodeType::TYPE_BOOL:
        typeString = "Bool";
        break;
    }
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + typeString + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";

    if(nameList != nullptr) {
        for(Identifier* i : *nameList) {
            nodeNum += 1;
            *dot += "n" + std::to_string(nodeNum) + " [label=" + i->className + "]\n";
            *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
            i->dotGen(dot);
        }
    }
    return;
}

void FuncDecNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;

    std::string typeString;
    switch (type) {
    case FuncType::FUNC_INT:
        typeString = "Integer";
        break;
    case FuncType::FUNC_REAL:
        typeString = "Double";
        break;
    case FuncType::FUNC_CHAR:
        typeString = "Char";
        break;
    case FuncType::FUNC_BOOL:
        typeString = "Bool";
        break;
    case FuncType::FUNC_VOID:
        typeString = "Void";
        break;
    }
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + typeString + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";

    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + name + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    if(argList != nullptr) {
        for(std::pair<NodeType, Identifier*> p : *argList) {
            nodeNum += 1;
            *dot += "n" + std::to_string(nodeNum) + " [label=" + p.second->className + "]\n";
            *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
            p.second->dotGen(dot);
        }
    }
    
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + body->className + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    body->dotGen(dot);
    return;
}

void BinaryExprNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    if(lhs != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + lhs->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        lhs->dotGen(dot);
    }
    std::string opString = "";
    switch (op) {
    case BinaryOperator::OP_PLUS:
        opString = "+";
        break;
    case BinaryOperator::OP_MINUS:
        opString = "-";
        break;
    case BinaryOperator::OP_MUL:
        opString = "*";
        break;
    case BinaryOperator::OP_DIV:
        opString = "/";
        break;
    case BinaryOperator::OP_GE:
        opString = ">=";
        break;
    case BinaryOperator::OP_GT:
        opString = ">";
        break;
    case BinaryOperator::OP_LT:
        opString = "<";
        break;
    case BinaryOperator::OP_LE:
        opString = "<=";
        break;
    case BinaryOperator::OP_EQUAL:
        opString = "==";
        break;
    case BinaryOperator::OP_UNEQUAL:
        opString = "!=";
        break;
    case BinaryOperator::OP_OR:
        opString = "||";
        break;
    case BinaryOperator::OP_MOD:
        opString = "%";
        break;
    case BinaryOperator::OP_AND:
        opString = "&&";
        break;
    }
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=\"" + opString + "\"]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    if(rhs != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + rhs->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        rhs->dotGen(dot);
    }
    return;
}

void CallExprNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + callee + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    if(args != nullptr) {
        for(ExprNode* e: *args) {
            nodeNum += 1;
            *dot += "n" + std::to_string(nodeNum) + " [label=" + e->className + "]\n";
            *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
            e->dotGen(dot);
        }
    }
    return;
}

void ScanNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    if(args != nullptr) {
        for(Identifier* i : *args) {
            nodeNum += 1;
            *dot += "n" + std::to_string(nodeNum) + " [label=" + i->className + "]\n";
            *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
            i->dotGen(dot);
        }
    }
    
    return;
}

void PrintNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    if(args != nullptr) {
        for(std::pair<PrintItem, bool> p : *args) {
            nodeNum += 1;
            if(p.second) {
                *dot += "n" + std::to_string(nodeNum) + " [label=" + p.first.exp->className + "]\n";
                *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
                p.first.exp->dotGen(dot);
            } else {
                *dot += "n" + std::to_string(nodeNum) + " [label=\"" + *p.first.str + "\"]\n";
                *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
            }
            
        }
    }
    return;
}

void ReturnNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    if(res != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + res->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        res->dotGen(dot);
    }
    return;
}

void AssignStmtNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    if(lhs != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + lhs->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        lhs->dotGen(dot);
    }
    if(rhs != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + rhs->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        rhs->dotGen(dot);
    }
    return;
}

void IfStmtNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    if(thenStmt != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + thenStmt->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        thenStmt->dotGen(dot);
    }
    
    if(condition != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + condition->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        condition->dotGen(dot);
    }

    if(elseStmt != nullptr) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + elseStmt->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        elseStmt->dotGen(dot);
    }
    return;
}

void WhileStmtNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + condition->className + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    condition->dotGen(dot);
    nodeNum += 1;
    *dot += "n" + std::to_string(nodeNum) + " [label=" + staments->className + "]\n";
    *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
    staments->dotGen(dot);
    return;
}

void CompoundStmtNode::dotGen(std::string *dot) {
    std::string fromName = className;
    int rootNum = nodeNum;
    for(StmtNode* s : *stmtList) {
        nodeNum += 1;
        *dot += "n" + std::to_string(nodeNum) + " [label=" + s->className + "]\n";
        *dot += "n" + std::to_string(rootNum) + "->n" + std::to_string(nodeNum) + "\n";
        s->dotGen(dot);
    }
    return;
}

void BreakNode::dotGen(std::string *dot) {
    return;
}
void ContinueNode::dotGen(std::string *dot) {
    return;
}