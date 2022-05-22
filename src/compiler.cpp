#include <fstream>
#include "ast/ast.h"
#include "parser/grammar.hpp"
#include "ast/visualization.h"
#include "code_gen/code_gen.h"

extern int yyparse();
Node* Root;

int main()
{
    //parser
    yyparse();

    //visualize
    std::string* dotStr = new std::string();
    Visualization* vis = new Visualization();
    vis->dotGen(Root, dotStr);
    std::ofstream fout("ast/ast_vis.dot");
    fout << *dotStr;
    fout.close();

    //code gen
    generator = new Generator();
    generator->generate(Root);
    return 0;
}