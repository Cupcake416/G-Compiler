#include <iostream>
#include "code_gen.h"

int main()
{
    generator = new Generator();
    std::vector<StmtNode*> rootList, mainList;
    std::vector<ExprNode*> prinList;

    // TODO: string with bug
    // StringNode* str = new StringNode("Hello world");
    // prinList.push_back(str);
    std::vector<Identifier*> nameList;
    CharExprNode* line = new CharExprNode('\n');
    Identifier* i = new Identifier("i");
    DoubleExprNode* l = new DoubleExprNode(5);
    IntegerExprNode* r = new IntegerExprNode(3);
    BinaryExprNode* op = new BinaryExprNode(BinaryExprNode::OP_PLUS, i, r);
    nameList.push_back(i);
    prinList.push_back(i);
    prinList.push_back(line);
    
    VariableDeclNode* dec_i = new VariableDeclNode(&nameList, TYPE_INT);
    AssignStmtNode* ass_i = new AssignStmtNode(i, l);
    AssignStmtNode* ass_ii = new AssignStmtNode(i, op);
    CallExprNode* prin = new CallExprNode(new Identifier("print"), &prinList);
    mainList.push_back(ass_i);
    mainList.push_back(ass_ii);
    mainList.push_back(prin);

    CompoundStmtNode* mainBody = new CompoundStmtNode(&mainList);
    FuncDecNode* main = new FuncDecNode(new Identifier("main"), FUNC_VOID, nullptr, mainBody);
    rootList.push_back(dec_i);
    rootList.push_back(main);

    CompoundStmtNode* root = new CompoundStmtNode(&rootList);
    generator->generate(root);
    return 0;
}