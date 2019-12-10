%{
    #include "interp_bison.hpp"
%}

%option yylineno
%option noyywrap

%%

[/][/].*\n              ;
[0-9]+                  {
                            yylval = *yytext;
                            return NUM;
                        }
[a-zA-Z_][a-zA-Z0-9_]   {
                            yylval = *yytext;
                            return VAR;
                        }
[ \t\r\n]               ;
if                      return IF;
else                    return ELSE;
==                      return EQ;
[<]=                    return LESS_EQ;
>=                      return GR_EQ;
[<]                     return LESS;
>                       return GR;
&&                      return AND;
[|][|]                      return OR;
=                       return ASSIGN;

[-*+/]                  return *yytext;


%%
