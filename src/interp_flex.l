%{
    #include <string>
    #define YYSTYPE std::string
    #include "interp_bison.hpp"
    void yyerror(char *s);
%}

%option yylineno
%option noyywrap

%%

[/][/].*\n              ;
[0-9]+                  {
                            yylval = *yytext;
                            return NUM;
                        }
[ \t\r\n]               ;
int                     return INT;
if                      return IF;
else                    return ELSE;
[a-zA-Z_][a-zA-Z0-9_]*  {
                            yylval = *yytext;
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
.                       yyerror("Invalid character");
%%
