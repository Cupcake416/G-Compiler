%{
    #include <cstdio>
    #include <string>
    #include "../ast/ast.h"
    #include "grammar.hpp"
    extern int line;
    int str2ch(std::string str);
    std::string strText() {std::string s = yytext; return s;}
%}
COMMENT \/\/[^\n]*
digits [0-9]+

CHAR  \'.\'|\'\\.\'
STRING \"(\\.|[^"\\])*\"
INT -?(0|[1-9][0-9]*)
REAL -?({digits}\.{digits})
BOOL "true"|"false"
TYPE "int"|"double"|"bool"|"char"

ID [_a-zA-Z][_0-9a-zA-Z]*

OP_0 "*"|"/"
OP_1 "%"
OP_2 "+"|"-"
OP_3 ">"|"<"|">="|"<="|"=="|"!="
OP_4 "&&"
OP_5 "||"


LP \(
RP \)
LB \[
RB \]
LC \{
RC \}

CONST const
RETURN return
IF if
ELSE else
WHILE while
BREAK break
CONTINUE continue
VOID void
SCAN scan
PRINT print

SEMI ;
COMMA ,
ASSIGNOP =

BLANK \t|" "|\r|\n

%%
{CHAR}  {
    if(str2ch(strText()) == 128)
        fprintf(stderr, "Line %d: Invalid characters %s\n", line, yytext);
    else 
    {
        yylval.charVal = str2ch(strText());
        return CHAR;
    }
}
{INT}   {yylval.intVal = std::stoi(strText()); return INT;}
{REAL} {yylval.doubleVal = std::stof(strText()); return REAL;}   
{BOOL}  {yylval.boolVal = strText() == "true"; return BOOL;}
{STRING}    {
    yylval.strVal = new std::string();
    std::string s = strText();
    for(int i = 1; i < s.length() - 1; i++)
    {
        if(s[i] != '\\') yylval.strVal->push_back(s[i]);
        else 
        {
            int c = str2ch(s.substr(i - 1, i + 2));
            if(c == 128) fprintf(stderr, "Line %d: Invalid characters %s\n", line, yytext);
            else yylval.strVal->push_back(c);
            i++;
        }
    }
    return STRING;
}

{TYPE}   {
    yylval.ty = strText() == "int" ? TYPE_INT :
                strText() == "double" ? TYPE_REAL :
                strText() == "char" ? TYPE_CHAR : TYPE_BOOL;
    return TYPE;
}

{OP_0}   {yylval.op = strText() == "*" ? OP_MUL : OP_DIV; return OP_0;}
{OP_1}   {yylval.op = OP_MOD; return OP_1;}
{OP_2}   {yylval.op = strText() == "+" ? OP_PLUS : OP_MINUS; return OP_2;}
{OP_3}   {
    yylval.op = strText() == ">" ? OP_GT :
                strText() == "<" ? OP_LT :
                strText() == ">=" ? OP_GE :
                strText() == "<=" ? OP_LE :
                strText() == "==" ? OP_EQUAL : OP_UNEQUAL;
    return OP_3;
}
{OP_4}   {yylval.op = OP_AND; return OP_4;}
{OP_5}   {yylval.op = OP_OR; return OP_5;}

{LP}   {return LP;}
{RP}   {return RP;}
{LB}   {return LB;}
{RB}   {return RB;}
{LC}   {return LC;}
{RC}   {return RC;}

{CONST}   {return CONST;}
{IF}   {return IF;}
{ELSE}   {return ELSE;}
{WHILE}   {return WHILE;}
{BREAK}   {return BREAK;}
{CONTINUE}   {return CONTINUE;}
{VOID}   {return VOID;}
{RETURN}   {return RETURN;}
{SCAN}   {return SCAN;}
{PRINT}   {return PRINT;}

{SEMI}   {return SEMI;}
{COMMA}   {return COMMA;}
{ASSIGNOP}   {return ASSIGNOP;}

{ID}  {yylval.id = new std::string(strText()); return ID;}

{BLANK}  {if(strText() == "\n") line++;}
{COMMENT} {;}

.   {fprintf(stderr, "Line %d: Invalid characters \'%s\'\n", line, yytext);}

%%
int yywrap() {return 1;}
int str2ch(std::string str)
{
    if(str[1] != '\\') return str[1];
    else if (str[2] == 'n') return '\n';
    else if (str[2] == 'r') return '\r';
    else if (str[2] == 't') return '\t';
    else if (str[2] == '\\') return '\\';
    else if (str[2] == '\'') return '\'';
    else if (str[2] == '"') return '\"';
    else if (str[2] == '0') return '\0';
    else return 128;
}