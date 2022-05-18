#include <iostream>
#include "code_gen.h"

int main()
{
    generator = new Generator();
    std::vector<StmtNode*> rootList, mainList, loopList;
    std::vector<std::pair<PrintNode::PrintItem, bool> > prinList;
    std::vector<Identifier*> scanList;

    PrintNode::PrintItem item;
    item.str = new std::string("Hello, ");
    prinList.push_back(make_pair(item, false));
    
    std::vector<Identifier*> nameList;
    Identifier* a = new Identifier("a", nullptr, 10);
    nameList.push_back(a);
    
    Identifier* a_i = new Identifier("a", new IntegerExprNode(1));
    scanList.push_back(a_i);
    Identifier* b = new Identifier("b");
    nameList.push_back(b);
    item.exp = b;
    prinList.push_back(make_pair(item, true));
    CharExprNode* line = new CharExprNode('\n');
    item.exp = line;
    prinList.push_back(make_pair(item, true));
    
    ScanNode* scan = new ScanNode(&scanList);
    AssignStmtNode* ass_1 = new AssignStmtNode(b, new IntegerExprNode(1));
    AssignStmtNode* ass_2 = new AssignStmtNode(b, new BinaryExprNode(BinaryExprNode::OP_MUL, b, new IntegerExprNode(2)));
    IfStmtNode* if1 = new IfStmtNode(new BinaryExprNode(BinaryExprNode::OP_LT, b, new IntegerExprNode(60)), new ContinueNode(), nullptr);
    PrintNode* prin = new PrintNode(&prinList);
    loopList.push_back(ass_2);
    loopList.push_back(if1);
    loopList.push_back(prin);
    CompoundStmtNode* loopBody = new CompoundStmtNode(&loopList);
    WhileStmtNode* loop1 = new WhileStmtNode(new BinaryExprNode(BinaryExprNode::OP_LE, b, a_i), loopBody);
    mainList.push_back(scan);
    mainList.push_back(ass_1);
    mainList.push_back(loop1);

    VariableDeclNode* dec_i = new VariableDeclNode(&nameList, TYPE_INT);
    CompoundStmtNode* mainBody = new CompoundStmtNode(&mainList);
    FuncDecNode* main = new FuncDecNode(new Identifier("main"), FUNC_VOID, nullptr, mainBody);
    rootList.push_back(dec_i);
    rootList.push_back(main);

    CompoundStmtNode* root = new CompoundStmtNode(&rootList);
    generator->generate(root);
    return 0;
}