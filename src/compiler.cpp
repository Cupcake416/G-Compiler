#include <iostream>
#include "ast/ast.h"
#include "parser/grammar.hpp"
#include "code_gen/code_gen.h"
#include "ast/visualization.h"
#include <fstream>

extern int yyparse();
Node* Root;

int main()
{
    std::string dot = "";
    yyparse();
    generator = new Generator();
    generator->generate(Root);
    Visualization* vis = new Visualization();
    std::ofstream os("./ast/ast_vis.dot");
    vis->dotGen(Root, &dot);
    os << dot;
    os.close();
    return 0;
}