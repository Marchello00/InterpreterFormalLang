%{
    #include <syntax_tree.h>
    #include <iostream>
    #include <cstdlib>

    extern int yylineno;
    extern int yylex();

    void yyerror(CmdListNode **root, const std::string &s) {
        std::cerr << s << "\n";
        exit(1);
    }

    typedef struct {
        std::string str;
        ExpressionNode *expr;
        CmdNode *cmd;
        CmdListNode *cmd_list;
        OperatorNode  *oper;
        TypeNode *var_type;
    } YYSTYPE_struct;

    #define YYSTYPE YYSTYPE_struct
%}

%token IF ELSE
%token EQ LESS GR LESS_EQ GR_EQ NOT_EQ NOT AND OR
%token ASSIGN
%token INT VAR NUM

%type<cmd> CMD
%type<cmd_list> CMDS
%type<var_type> VAR_TYPE
%type<expr> EXPR CREATING ASSIGNING LOGIC_EXPR
%type<expr> LOGIC_AND_EXPR LOGIC_CMP_EXPR LOGIC_FINAL_EXPR
%type<expr> ARITH_EXPR ARITH_MUL_EXPR ARITH_FINAL_EXPR
%type<str> VAR NUM

%parse-param {CmdListNode **root}

%%
PROGRAM:                CMDS                                    {*root = $1}
CMDS:                   CMDS CMD                                {
                                                                    $$ = $1;
                                                                    $$->addCmd($2);
                                                                }
|                       CMD                                     {$$ = new CmdListNode($1);}
;
CMD:                    '{' CMDS '}'                            {$$ = new CmdNode($2);}
|                       EXPR ';'                                {$$ = new CmdNode($1); $$->setSimple();}
|                       IF '(' EXPR ')' CMD ELSE CMD            {$$ = new CmdNode(new IfOperatorNode($3, $5, $7));}
;
EXPR:                   LOGIC_EXPR
|                       ASSIGNING
|                       CREATING
;
CREATING:               VAR_TYPE VAR ASSIGN EXPR                {$$ = new CreateOperator($1, $2, $4);}
|                       VAR_TYPE VAR                            {$$ = new CreateOperator($1, $2);}
;
ASSIGNING:              VAR ASSIGN EXPR                         {$$ = new AssignOperator($1, $3);}
;
VAR_TYPE:               INT                                     {$$ = new TypeNode(TypeIdentifyer::INT_T);}
;
LOGIC_EXPR:             LOGIC_AND_EXPR
|                       LOGIC_EXPR OR LOGIC_AND_EXPR            {$$ = new OrOperator($1, $3);}
;
LOGIC_AND_EXPR:         LOGIC_CMP_EXPR
|                       LOGIC_AND_EXPR AND LOGIC_CMP_EXPR       {$$ = new AndOperator($1, $3);}
;
LOGIC_CMP_EXPR:         LOGIC_FINAL_EXPR
|                       LOGIC_CMP_EXPR EQ LOGIC_FINAL_EXPR      {$$ = new EqOperator($1, $3);}
|                       LOGIC_CMP_EXPR LESS LOGIC_FINAL_EXPR    {$$ = new LessOperator($1, $3);}
|                       LOGIC_CMP_EXPR GR LOGIC_FINAL_EXPR      {$$ = new GrOperator($1, $3);}
|                       LOGIC_CMP_EXPR LESS_EQ LOGIC_FINAL_EXPR {$$ = new LessEqOperator($1, $3);}
|                       LOGIC_CMP_EXPR GR_EQ LOGIC_FINAL_EXPR   {$$ = new GrEqOperator($1, $3);}
|                       LOGIC_CMP_EXPR NOT_EQ LOGIC_FINAL_EXPR  {$$ = new NotEqOperator($1, $3);}
;
LOGIC_FINAL_EXPR:       '(' EXPR ')'                            {$$ = $2;}
|                       ARITH_EXPR
|                       NOT LOGIC_FINAL_EXPR                    {$$ = new NotOperator($2);}
;
ARITH_EXPR:             ARITH_MUL_EXPR
|                       ARITH_EXPR '+' ARITH_MUL_EXPR           {$$ = new PlusOperator($1, $3);}
|                       ARITH_EXPR '-' ARITH_MUL_EXPR           {$$ = new MinusOperator($1, $3);}
;
ARITH_MUL_EXPR:         ARITH_FINAL_EXPR
|                       ARITH_FINAL_EXPR '*' ARITH_MUL_EXPR     {$$ = new MultOperator($1, $3);}
|                       ARITH_FINAL_EXPR '/' ARITH_MUL_EXPR     {$$ = new DivideOperator($1, $3);}
;
ARITH_FINAL_EXPR:       '(' EXPR ')'                            {$$ = $2}
|                       NUM                                     {$$ = new IntValueNode($1.c_str());}
|                       VAR                                     {$$ = new VariableNode($1);}
|                       '-' ARITH_FINAL_EXPR                    {$$ = new UnaryMinusOperator($2);}
;
%%
