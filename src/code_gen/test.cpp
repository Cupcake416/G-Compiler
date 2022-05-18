#include <iostream>
#include "code_gen.h"

int main()
{
    generator = new Generator();
    std::vector<StmtNode*> rootList, mainList;
    std::vector<std::pair<PrintNode::PrintItem, bool> > prinList;

    PrintNode::PrintItem item;
    item.str = new std::string("Hello, ");
    prinList.push_back(make_pair(item, false));
    
    std::vector<Identifier*> nameList;
    Identifier* a = new Identifier("a", nullptr, 10);
    nameList.push_back(a);
    
    Identifier* a_i = new Identifier("a", new IntegerExprNode(1));
    item.exp = a_i;
    prinList.push_back(make_pair(item, true));
    CharExprNode* line = new CharExprNode('\n');
    item.exp = line;
    prinList.push_back(make_pair(item, true));
    
    VariableDeclNode* dec_i = new VariableDeclNode(&nameList, TYPE_INT);
    AssignStmtNode* ass_i = new AssignStmtNode(a_i, new BinaryExprNode(BinaryExprNode::OP_PLUS, a_i, new IntegerExprNode(1)));
    PrintNode* prin = new PrintNode(&prinList);
    mainList.push_back(dec_i);
    mainList.push_back(ass_i);
    mainList.push_back(prin);

    CompoundStmtNode* mainBody = new CompoundStmtNode(&mainList);
    FuncDecNode* main = new FuncDecNode(new Identifier("main"), FUNC_VOID, nullptr, mainBody);
    rootList.push_back(main);

    CompoundStmtNode* root = new CompoundStmtNode(&rootList);
    generator->generate(root);
    return 0;
}