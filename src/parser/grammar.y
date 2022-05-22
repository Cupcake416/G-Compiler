%{
    #include <cstdio>
    #include "../ast/ast.h"
    int yylex(void);
    int line = 1;
    extern Node *Root;
    void yyerror(char* s) {fprintf(stderr, "%s\n", s);}
%}

%union
{
    int intVal;
    double doubleVal;
    char charVal;
    bool boolVal;
    std::string* strVal;
    NodeType ty;
    BinaryOperator op;
    std::string* id;
    ExprNode* expr;
    StmtNode* stmt;
    ConstExprNode* constExpr;
    Identifier* identifier;
    std::vector<Identifier*>* idList;
    std::vector<ExprNode*>* paramList;
    std::vector<StmtNode*>* stmtList;
    std::vector<std::pair<NodeType, Identifier*> > *argList;
    std::vector<std::pair<PrintItem, bool> > *printList;
    IntegerExprNode* intExpr;
    DoubleExprNode* doubleExpr;
    CharExprNode* charExpr;
    BooleanExprNode* boolExpr;
    ConstDeclNode* constDec;
    VariableDeclNode* varDec;
    FuncDecNode* funcDec;
    BinaryExprNode* binExpr;
    CallExprNode* callExpr;
    AssignStmtNode* assignStmt;
    IfStmtNode* ifStmt;
    ReturnNode* retStmt;
    WhileStmtNode* whileStmt;
    CompoundStmtNode* stmts;
    ScanNode* scanStmt;
    PrintNode* printStmt;
}
%token <intVal> INT
%token <doubleVal> REAL
%token <charVal> CHAR
%token <boolVal> BOOL
%token <strVal> STRING
%token <ty> TYPE
%token <id> ID
%token <op> OP_0
%token <op> OP_1
%token <op> OP_2
%token <op> OP_3
%token <op> OP_4
%token <op> OP_5
%token LP RP LB RB LC RC RETURN CONST IF ELSE WHILE BREAK CONTINUE VOID SEMI COMMA ASSIGNOP SCAN PRINT

%nonassoc NO_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OP_5
%left OP_4
%left OP_3
%left OP_2
%left OP_1
%left OP_0
%left LP RP LB RB

%type <expr> Expr
%type <stmt> Stmt
%type <constExpr> ConstExpr
%type <identifier> Iden IdenDef
%type <idList> ScanList VarDecList
%type <intExpr> IntExpr
%type <doubleExpr> DoubleExpr
%type <charExpr> CharExpr
%type <boolExpr> BoolExpr
%type <constDec> ConstDec
%type <varDec> VarDec
%type <funcDec> FunDec
%type <paramList> ParamList
%type <printList> PrintList
%type <stmtList> StmtList
%type <argList> ArgList
%type <binExpr> BinExpr
%type <callExpr> CallExpr
%type <assignStmt> AssignStmt
%type <ifStmt> IfStmt
%type <retStmt> RetStmt
%type <whileStmt> WhileStmt
%type <scanStmt> ScanStmt
%type <printStmt> PrintStmt
%type <stmts> Program MainList Stmts

%%
Program: MainList {$$ = $1; Root = $$;}
    ;

MainList: MainList VarDec {$$ = $1; $$->stmtList->push_back($2);}
    | MainList FunDec {$$ = $1; $$->stmtList->push_back($2);}
    | MainList ConstDec {$$ = $1; $$->stmtList->push_back($2);}
    | VarDec {$$ = new CompoundStmtNode(line, new std::vector<StmtNode*>); $$->stmtList->push_back($1);}
    | FunDec {$$ = new CompoundStmtNode(line, new std::vector<StmtNode*>); $$->stmtList->push_back($1);}
    | ConstDec {$$ = new CompoundStmtNode(line, new std::vector<StmtNode*>); $$->stmtList->push_back($1);}
    ;

ScanStmt: SCAN LP ScanList RP SEMI {$$ = new ScanNode(line, $3);}
    ;

PrintStmt: PRINT LP PrintList RP SEMI {$$ = new PrintNode(line, $3);}
    ;

VarDec: TYPE VarDecList SEMI {$$ = new VariableDeclNode(line, $2, $1);}
    ;

VarDecList: VarDecList COMMA IdenDef {$$ = $1; $$->push_back($3);}
    | IdenDef {$$ = new std::vector<Identifier*>; $$->push_back($1);}

Iden: ID {$$ = new Identifier(line, *$1);}
    | ID LB Expr RB {$$ = new Identifier(line, *$1, $3);}
    ;

IdenDef: ID {$$ = new Identifier(line, *$1);}
    | ID LB INT RB {$$ = new Identifier(line, *$1, nullptr, $3);}
    ;

Expr: Iden {$$ = $1;}
    | ConstExpr {$$ = $1;}
    | BinExpr {$$ = $1;}
    | CallExpr {$$ = $1;}
    | LP Expr RP {$$ = $2;}
    ;

BinExpr: Expr OP_0 Expr {$$ = new BinaryExprNode(line, $2, $1, $3);}
    | Expr OP_1 Expr {$$ = new BinaryExprNode(line, $2, $1, $3);}
    | Expr OP_2 Expr {$$ = new BinaryExprNode(line, $2, $1, $3);}
    | Expr OP_3 Expr {$$ = new BinaryExprNode(line, $2, $1, $3);}
    | Expr OP_4 Expr {$$ = new BinaryExprNode(line, $2, $1, $3);}
    | Expr OP_5 Expr {$$ = new BinaryExprNode(line, $2, $1, $3);}
    ;

CallExpr: ID LP ParamList RP {$$ = new CallExprNode(line, *$1, $3);}
    | ID LP RP {$$ = new CallExprNode(line, *$1, nullptr);}
    ;

ParamList: ParamList COMMA Expr {$$ = $1; $$->push_back($3);}
    | Expr {$$ = new std::vector<ExprNode*>; $$->push_back($1);}
    ;

ConstDec: CONST TYPE IdenDef ASSIGNOP ConstExpr SEMI {$$ = new ConstDeclNode(line, $3, $5, $2);}
    ;

ConstExpr: IntExpr {$$ = $1;}
    | DoubleExpr {$$ = $1;}
    | CharExpr {$$ = $1;}
    | BoolExpr {$$ = $1;}
    ;

IntExpr: INT {$$ = new IntegerExprNode(line, $1);}
    ;

DoubleExpr: REAL {$$ = new DoubleExprNode(line, $1);}
    ;

CharExpr: CHAR {$$ = new CharExprNode(line, $1);}
    ;

BoolExpr: BOOL {$$ = new BooleanExprNode(line, $1);}
    ;

FunDec: TYPE ID LP ArgList RP Stmts {$$ = new FuncDecNode(line, *$2, FuncType((int)$1), $4, $6);}
    | VOID ID LP ArgList RP Stmts {$$ = new FuncDecNode(line, *$2, FUNC_VOID, $4, $6);}
    | TYPE ID LP RP Stmts {$$ = new FuncDecNode(line, *$2, FuncType((int)$1), nullptr, $5);}
    | VOID ID LP RP Stmts {$$ = new FuncDecNode(line, *$2, FUNC_VOID, nullptr, $5);}
    ;

ArgList: ArgList COMMA TYPE IdenDef {$$ = $1; $$->push_back(std::make_pair($3, $4));}
    | TYPE IdenDef {$$ = new std::vector<std::pair<NodeType, Identifier*> >; $$->push_back(std::make_pair($1, $2));} 
    ;

Stmts: LC StmtList RC {$$ = new CompoundStmtNode(line, $2);}
    | LC RC {$$ = new CompoundStmtNode(line, nullptr);}
    ;

StmtList: StmtList Stmt {$$ = $1; $$->push_back($2);}
    | Stmt {$$ = new std::vector<StmtNode*>; $$->push_back($1);}
    ;

Stmt: Stmts {$$ = $1;}
    | Expr SEMI {$$ = $1;}
    | ConstDec {$$ = $1;}
    | VarDec {$$ = $1;}
    | ScanStmt {$$ = $1;}
    | PrintStmt {$$ = $1;}
    | AssignStmt {$$ = $1;}
    | RetStmt {$$ = $1;}
    | IfStmt {$$ = $1;}
    | WhileStmt {$$ = $1;}
    | BREAK SEMI {$$ = new BreakNode(line);}
    | CONTINUE SEMI {$$ = new ContinueNode(line);}
    ;

ScanList: ScanList COMMA Iden {$$ = $1; $$->push_back($3);}
    | Iden {$$ = new std::vector<Identifier*>; $$->push_back($1);}
    ;

PrintList: PrintList COMMA Expr {
        PrintItem it;
        it.exp = $3;
        $$ = $1;
        $$->push_back(std::make_pair(it, true));
    }
    | PrintList COMMA STRING {
        PrintItem it;
        it.str = $3;
        $$ = $1;
        $$->push_back(std::make_pair(it, false));
    }
    | Expr {
        $$ = new std::vector<std::pair<PrintItem, bool> >;
        PrintItem it;
        it.exp = $1;
        $$->push_back(std::make_pair(it, true));
    }
    | STRING {
        $$ = new std::vector<std::pair<PrintItem, bool> >;
        PrintItem it;
        it.str = $1;
        $$->push_back(std::make_pair(it, false));
    }
    ;

AssignStmt: Iden ASSIGNOP Expr SEMI {$$ = new AssignStmtNode(line, $1, $3);}
    ;

RetStmt: RETURN Expr SEMI {$$ = new ReturnNode(line, $2);}
    | RETURN SEMI {$$ = new ReturnNode(line, nullptr);}
    ;

IfStmt: IF LP Expr RP Stmt ELSE Stmt {$$ = new IfStmtNode(line, $3, $5, $7);}
    | IF LP Expr RP Stmt %prec NO_ELSE {$$ = new IfStmtNode(line, $3, $5, nullptr);}
    ;

WhileStmt: WHILE LP Expr RP Stmt {$$ = new WhileStmtNode(line, $3, $5);}
    ;

%%
