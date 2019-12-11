%{
    #include <iostream>

    extern int yylineno;
    extern int yylex();

    void yyerror(char *s) {
        std::cerr << s << "\n";
        exit(1);
    }

    #define YYSTYPE std::string
%}

%token IF ELSE
%token EQ LESS GR LESS_EQ GR_EQ NOT_EQ NOT AND OR
%token ASSIGN
%token INT
%token VAR NUM

%%

PROGRAM:                CMDS
;
CMDS:                   CMDS CMD
|                       CMD
;
CMD:                    '{' CMDS '}'
|                       EXPR ';'
|                       IF '(' EXPR ')' CMD ELSE CMD
;
EXPR:                   LOGIC_EXPR
|                       ASSIGNING
|                       CREATING
;
CREATING:               VAR_TYPE VAR ASSIGN EXPR
|                       VAR_TYPE VAR
;
ASSIGNING:              VAR ASSIGN EXPR
;
VAR_TYPE:               INT
;
LOGIC_EXPR:             LOGIC_AND_EXPR
|                       LOGIC_EXPR OR LOGIC_AND_EXPR
;
LOGIC_AND_EXPR:         LOGIC_CMP_EXPR
|                       LOGIC_AND_EXPR AND LOGIC_CMP_EXPR
;
LOGIC_CMP_EXPR:         LOGIC_FINAL_EXPR
|                       LOGIC_CMP_EXPR EQ LOGIC_FINAL_EXPR
|                       LOGIC_CMP_EXPR LESS LOGIC_FINAL_EXPR
|                       LOGIC_CMP_EXPR GR LOGIC_FINAL_EXPR
|                       LOGIC_CMP_EXPR LESS_EQ LOGIC_FINAL_EXPR
|                       LOGIC_CMP_EXPR GR_EQ LOGIC_FINAL_EXPR
|                       LOGIC_CMP_EXPR NOT_EQ LOGIC_FINAL_EXPR
;
LOGIC_FINAL_EXPR:       '(' EXPR ')'
|                       ARITH_EXPR
|                       NOT LOGIC_FINAL_EXPR
;
ARITH_EXPR:             ARITH_MUL_EXPR
|                       ARITH_EXPR '+' ARITH_MUL_EXPR
|                       ARITH_EXPR '-' ARITH_MUL_EXPR
;
ARITH_MUL_EXPR:         ARITH_FINAL_EXPR
|                       ARITH_FINAL_EXPR '*' ARITH_MUL_EXPR
|                       ARITH_FINAL_EXPR '/' ARITH_MUL_EXPR
;
ARITH_FINAL_EXPR:       '(' EXPR ')'
|                       '-' '(' EXPR ')'
|                       NUM
|                       '-' NUM
|                       VAR
|                       '-' VAR
;
%%

int main() {
    yydebug = 0;
    return yyparse();
}
