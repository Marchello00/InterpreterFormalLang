%{
    #include <iostream>

    extern int yylineno;
    extern int yylex();

    void yyerror(char *s) {
        std::cerr << s << "\n";
    }
%}

%token IF ELSE
%token EQ LESS GR LESS_EQ GR_EQ NOT_EQ NOT AND OR
%token ASSIGN
%token VAR NUM
%token EOL

%%



%%

int main() {
    return yyparse();
}
