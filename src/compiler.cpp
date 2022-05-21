#include <iostream>
#include "ast/ast.h"
#include "parser/grammar.hpp"
#include "code_gen/code_gen.h"

extern int yyparse();
Node* Root;

int main()
{
    yyparse();
    generator = new Generator();
    generator->generate(Root);
    return 0;
}