%{
    #include <string>
    #define YYSTYPE std::string
    #include "interp_bison.hpp"
    #include <syntax_tree.h>
    void yyerror(CmdListNode **root, const std::string &s);
%}

%option yylineno
%option noyywrap

%%

[/][/].*\n              ;
[0-9]+                  {
                            yylval = yytext;
                            return NUM;
                        }
[ \t\r\n]               ;
read_int                return READ_INT;
write                   return WRITE;
int                     return INT;
if                      return IF;
else                    return ELSE;
[a-zA-Z_][a-zA-Z0-9_]*  {
                            yylval = yytext;
                            return VAR;
                        }
==                      return EQ;
[<]=                    return LESS_EQ;
>=                      return GR_EQ;
[<]                     return LESS;
>                       return GR;
&&                      return AND;
[|][|]                  return OR;
=                       return ASSIGN;
[-*+/{};(),]            return *yytext;
.                       yyerror(nullptr, "Invalid character");
%%
