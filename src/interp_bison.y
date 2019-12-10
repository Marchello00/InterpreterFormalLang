%{
    #include <iostream>

    extern int yylineno;
    extern int yylex();

    void yyerror(char *s) {
        std::cerr << s << "\n";
    }

    #define YYSTYPE std::string
%}

%token IF ELSE
%token EQ LESS GR LESS_EQ GR_EQ NOT_EQ NOT AND OR
%token ASSIGN
%token VAR NUM

%%

PROGRAM: NUM

%%

int main() {
    return yyparse();
}
