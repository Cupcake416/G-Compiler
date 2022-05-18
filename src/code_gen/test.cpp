#include <iostream>
#include "code_gen.h"

int main()
{
    generator = new Generator();
    std::vector<StmtNode*> rootList, mainList;
    std::vector<std::pair<PrintNode::PrintItem, bool> > prinList;
    std::vector<Identifier*> scanList;

    PrintNode::PrintItem item;
    item.str = new std::string("Hello, ");
    prinList.push_back(make_pair(item, false));
    
    std::vector<Identifier*> nameList;
    Identifier* a = new Identifier("a", nullptr, 10);
    nameList.push_back(a);
    scanList.push_back(a);
    
    Identifier* a_i = new Identifier("a", new IntegerExprNode(1));
    Identifier* b = new Identifier("b");
    nameList.push_back(b);
    item.exp = b;
    prinList.push_back(make_pair(item, true));
    CharExprNode* line = new CharExprNode('\n');
    item.exp = line;
    prinList.push_back(make_pair(item, true));
    
    ScanNode* scan = new ScanNode(&scanList);
    AssignStmtNode* ass_i = new AssignStmtNode(b, new BinaryExprNode(BinaryExprNode::OP_PLUS, a_i, new IntegerExprNode(1)));
    PrintNode* prin = new PrintNode(&prinList);
    mainList.push_back(scan);
    mainList.push_back(ass_i);
    mainList.push_back(prin);

    VariableDeclNode* dec_i = new VariableDeclNode(&nameList, TYPE_CHAR);
    CompoundStmtNode* mainBody = new CompoundStmtNode(&mainList);
    FuncDecNode* main = new FuncDecNode(new Identifier("main"), FUNC_VOID, nullptr, mainBody);
    rootList.push_back(dec_i);
    rootList.push_back(main);

    CompoundStmtNode* root = new CompoundStmtNode(&rootList);
    generator->generate(root);
    return 0;
}