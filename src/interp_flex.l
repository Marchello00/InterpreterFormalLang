%{
    #include <val_bison.h>
%}

%yylineno
%noyywrap

%%

[\][\].*\n              ;
[0-9]+                  {
                            yylval = yytext;
                            return NUM;
                        }
[a-zA-Z_][a-zA-Z0-9_]   {
                            yyval = yytext;
                            return VAR;
                        }
[\n]                    return EOL;
[ \t\r]                 ;
if                      return IF;
else                    return ELSE;
==                      return EQ;
<=                      return LESS_EQ;
>=                      return GR_EQ;
<                       return LESS;
>                       return GR;
&&                      return AND;
||                      return OR;
=                       return ASSIGN;

[-*+/]                  return *yytext;


%%
