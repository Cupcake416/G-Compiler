## 词法分析

词法分析是将字符序列转换为标记(token)序列的过程。在词法分析阶段，编译器会先输入要求源程序字符串流，之后将字符流转换为规定好的标记序列，同时将所需要的信息存储起来，最后将结果交给语法分析器进行下一步编译的过程。

### 1 flex简介

flex（快速词法分析产⽣器，fast lexical analyzer generator）是⼀种词法分析程序。它是lex的开放源代码版本，以BSD许可证发布。通常与GNU bison⼀同运作。

标准lex文件由三部分组成，分别是定义区、规则区和用户子过程区。在定义区，用户可以编写C语言中的声明语句，导入需要的头文件或声明变量。在规则区，用户需要编写以正则表达式和对应的动作的形式的说明等价规则的代码。在用户子过程区自定义函数。完成一种如下形式的定义：

```
{definitions}
%%
{rules}
%%
{subroutines}
```

通过felx我们将完成以下状态转换过程：

![image-20220522210337116](C:\Users\59700\AppData\Roaming\Typora\typora-user-images\image-20220522210337116.png)

### 2 实现细节

本项⽬定义的token包括：C语⾔关键字if、else、return、while、int、float等每个关键字对应的token，加减乘除、位运算、括号、逗号、取地址等符号token，以及表示⽤户⾃定义的标志符（identifier）的token、数字常量token（整型和浮点数）、字符串常量token等。
我们需要先在yacc源文件grammar.y中声明这些token，并在lex源⽂件lexical.l中定义这些token对应的操作。
token大致分为如下几种情况：

```cpp
//关键词
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

//操作符
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
//标记符
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

{fprintf(stderr, "Line %d: Invalid characters \'%s\'\n", line, yytext);}
```

## 语法分析

语法分析（英语：syntactic analysis，也叫parsing）是根据某种给定的形式⽂法对由单词序列（如英语单词序列）构成的输⼊文本进行分析并确定其语法结构的⼀种过程。

语法分析器（parser）通常是作为编译器或解释器的组件出现的，它的作用是进行语法检查、并构建由输入的单词组成的数据结构（⼀般是语法分析树、抽象语法树等层次化的数据结构）。语法分析器通常使用独立的词法分析器从输⼊字符流中分离出⼀个个的“单词”，并将单词流作为其输入。开发中，语法分析器可以手写，也可以使用工具自定义生成。

### 1 YACC简介

YACC是Unix/Linux上用于生成编译器语言的编译器生成器，它使用BNF定义语法，能处理上下文无关文法(context-free)。YACC生成的编译器主要是用C语言写成的语法解析器(Parser)，需要与词法解析器Lex⼀起使用，并配合把两部分产生出来的C程序⼀并编译。定义标准与flex类似，也是由声明区、规则区、程序区三部分构成，完成一种如下形式的定义：

```
/* definitions */
 ....
%%
/* rules */
....
%%
/* auxiliary routines */
....
```
