#include <iostream>
#include "code_gen.h"

int main()
{
    // freopen("test.ll", "w", stdout);
    generator = new Generator();
    std::vector<StmtNode*> rootList, mainList;
    std::vector<ExprNode*> prinList;

    // StringNode* str = new StringNode("Hello world");
    // prinList.push_back(str);
    IntegerExprNode* i = new IntegerExprNode(0);
    prinList.push_back(i);
    
    CallExprNode* prin = new CallExprNode(new Identifier("print"), &prinList);
    mainList.push_back(prin);

    CompoundStmtNode* mainBody = new CompoundStmtNode(&mainList);
    FuncDecNode* main = new FuncDecNode(new Identifier("main"), FUNC_VOID, nullptr, mainBody);
    rootList.push_back(main);

    CompoundStmtNode* root = new CompoundStmtNode(&rootList);
    generator->generate(root);
    return 0;
}