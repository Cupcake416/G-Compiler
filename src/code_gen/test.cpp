#include <iostream>
#include "code_gen.h"

int main()
{
    generator = new Generator();
    std::vector<StmtNode*> rootList, mainList, funcList;
    std::vector<std::pair<PrintNode::PrintItem, bool> > prinList;
    std::vector<Identifier*> scanList;
    std::vector<ExprNode*> funcArgs, funcArgs1, funcArgs2;
    std::vector<std::pair<NodeType, Identifier*> > defArgs;

    PrintNode::PrintItem item;
    item.str = new std::string("Hello, ");
    prinList.push_back(make_pair(item, false));
    
    std::vector<Identifier*> nameList;
    Identifier* a = new Identifier("a", nullptr, 10);
    nameList.push_back(a);
    Identifier* a_1 = new Identifier("a", new IntegerExprNode(1));
    scanList.push_back(a_1);
    funcArgs.push_back(a_1);
    
    Identifier* x = new Identifier("x");
    defArgs.push_back(make_pair(TYPE_INT, x));
    Identifier* f = new Identifier("f");
    BinaryExprNode* cond = new BinaryExprNode(BinaryExprNode::OP_LE, x, new IntegerExprNode(1));
    funcList.push_back(new IfStmtNode(cond, new ReturnNode(new IntegerExprNode(1)), nullptr));
    funcArgs1.push_back(new BinaryExprNode(BinaryExprNode::OP_MINUS, x, new IntegerExprNode(1)));
    CallExprNode* val1 = new CallExprNode(f, &funcArgs1);
    funcArgs2.push_back(new BinaryExprNode(BinaryExprNode::OP_MINUS, x, new IntegerExprNode(2)));
    CallExprNode* val2 = new CallExprNode(f, &funcArgs2);
    funcList.push_back(new ReturnNode(new BinaryExprNode(BinaryExprNode::OP_PLUS, val1, val2)));
    
    item.exp = new CallExprNode(f, &funcArgs);
    prinList.push_back(make_pair(item, true));
    CharExprNode* line = new CharExprNode('\n');
    item.exp = line;
    prinList.push_back(make_pair(item, true));
    
    ScanNode* scan = new ScanNode(&scanList);
    PrintNode* prin = new PrintNode(&prinList);
    mainList.push_back(scan);
    mainList.push_back(prin);

    VariableDeclNode* dec_i = new VariableDeclNode(&nameList, TYPE_INT);
    CompoundStmtNode* funcBody = new CompoundStmtNode(&funcList);
    FuncDecNode* dec_f = new FuncDecNode(f, FUNC_INT, &defArgs, funcBody);
    CompoundStmtNode* mainBody = new CompoundStmtNode(&mainList);
    FuncDecNode* main = new FuncDecNode(new Identifier("main"), FUNC_VOID, nullptr, mainBody);
    rootList.push_back(dec_i);
    rootList.push_back(dec_f);
    rootList.push_back(main);

    CompoundStmtNode* root = new CompoundStmtNode(&rootList);
    generator->generate(root);
    return 0;
}