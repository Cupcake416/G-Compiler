#include <iostream>
#include <string>
#include "ast.h"

class Visualization {
public:
    void dotGen(Node *root, std::string* dot);
};