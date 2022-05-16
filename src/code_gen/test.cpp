#include <iostream>
#include "code_gen.h"

int main()
{
    std::vector<StmtNode*> rootList;

    FuncDecNode* main = new FuncDecNode(new Identifier("main"), FUNC_VOID, nullptr, nullptr);
    rootList.push_back(main);

    CompoundStmtNode* root = new CompoundStmtNode(&rootList);
    root->codeGen();
    return 0;
}