%{
    #include <syntax_tree.h>
    #include <iostream>
    #include <cstdlib>

    extern int yylineno;
    extern int yylex();

    void yyerror(char *s) {
        std::cerr << s << "\n";
        exit(1);
    }

    typedef struct {
        std::string str;
        ExpressionNode *expr;
        NodesListNode *cmds;
        OperatorNode  *oper;
    } YYSTYPE_struct;

    #define YYSTYPE YYSTYPE_struct
%}

%token IF ELSE
%token EQ LESS GR LESS_EQ GR_EQ NOT_EQ NOT AND OR
%token ASSIGN
%token INT VAR NUM

%type<cmds> CMDS
%type<expr> CMD EXPR CREATING ASSIGNING VAR_TYPE LOGIC_EXPR
%type<expr> LOGIC_AND_EXPR LOGIC_CMP_EXPR LOGIC_FINAL_EXPR
%type<expr> ARITH_EXPR ARITH_MUL_EXPR ARITH_FINAL_EXPR
%type<str> VAR NUM

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
ARITH_FINAL_EXPR:       '(' EXPR ')'                            {$$ = $2}
|                       NUM                                     {$$ = new IntValueNode($1);}
|                       VAR                                     {$$ = new VariableNode($1);}
|                       '-' ARITH_FINAL_EXPR                    {$$ = new UnaryMinusOperator($2);}
;
%%

int main() {
    yydebug = 0;
    return yyparse();
}
