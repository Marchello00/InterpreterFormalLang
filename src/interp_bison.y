%{
    #include <parser.h>
    #include <syntax_tree.h>
    #include <iostream>
    #include <cstdlib>

    extern int yylineno;
    extern int yylex();

    void yyerror(Interpreter *interpreter, const std::string &s) {
        std::cerr << s << "\n";
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

%token IF ELSE FOR WHILE
%token EQ LESS GR LESS_EQ GR_EQ NOT_EQ NOT AND OR
%token ASSIGN
%token INT VAR NUM
%token READ_INT WRITE EXIT

%type<cmd> CMD CMD1 CMD2
%type<cmd_list> CMDS
%type<var_type> VAR_TYPE
%type<expr> EEXPR EXPR CREATING ASSIGNING LOGIC_EXPR FUNCTION_CALL RET_FUNCTION_CALL
%type<expr> LOGIC_AND_EXPR LOGIC_CMP_EXPR LOGIC_FINAL_EXPR
%type<expr> ARITH_EXPR ARITH_MUL_EXPR ARITH_FINAL_EXPR
%type<str> VAR NUM

%parse-param {Interpreter *interpreter}

%%
PROGRAM:                PROGRAM CMD                                 {
                                                                        interpreter->set_node($2);
                                                                        interpreter->interpret();
                                                                        flex_interpreter.atStart = true;
                                                                    }
|                       CMD                                         {
                                                                        interpreter->set_node($1);
                                                                        interpreter->interpret();
                                                                        flex_interpreter.atStart = true;
                                                                    }
;
CMDS:                   CMDS CMD                                    {
                                                                        $$ = $1;
                                                                        $$->addCmd($2);
                                                                    }
|                       CMD                                         {$$ = new CmdListNode($1);}
;
CMD:                    CMD1
|                       CMD2
;
CMD1:                   '{' CMDS '}'                                {$$ = new CmdNode($2);}
|                       '{' '}'                                     {$$ = new CmdNode(new ExpressionNode());}
|                       FUNCTION_CALL ';'                           {$$ = new CmdNode($1); $$->setSimple();}
|                       EEXPR ';'                                   {$$ = new CmdNode($1); $$->setSimple();}
|                       IF '(' EEXPR ')' CMD1 ELSE CMD1             {$$ = new CmdNode(new IfOperatorNode($3, $5, $7));}
|                       WHILE '(' EEXPR ')' CMD1                    {$$ = new CmdNode(new WhileOperatorNode($3, $5));}
|                       FOR '(' EEXPR ';' EEXPR ';' EEXPR ')' CMD1  {$$ = new CmdNode(new ForOperatorNode($3, $5, $7, $9));}
;
CMD2:                   IF '(' EEXPR ')' CMD                        {$$ = new CmdNode(new IfOperatorNode($3, $5, nullptr));}
|                       IF '(' EEXPR ')' CMD1 ELSE CMD2             {$$ = new CmdNode(new IfOperatorNode($3, $5, $7));}
|                       WHILE '(' EEXPR ')' CMD2                    {$$ = new CmdNode(new WhileOperatorNode($3, $5));}
|                       FOR '(' EEXPR ';' EEXPR ';' EEXPR ')' CMD2  {$$ = new CmdNode(new ForOperatorNode($3, $5, $7, $9));}
;
EEXPR:                  EXPR
|                                                                   {$$ = new ExpressionNode();}
EXPR:                   LOGIC_EXPR
|                       ASSIGNING
|                       CREATING
;
RET_FUNCTION_CALL:      READ_INT '(' ')'                            {$$ = new ReadIntNode();}
FUNCTION_CALL:          WRITE '(' EXPR ')'                          {$$ = new WriteNode($3);}
|                       EXIT '(' ')'                                {$$ = new ExitNode();}
;
CREATING:               VAR_TYPE VAR ASSIGN EXPR                    {$$ = new CreateOperator($1, $2, $4);}
|                       VAR_TYPE VAR                                {$$ = new CreateOperator($1, $2);}
;
ASSIGNING:              VAR ASSIGN EXPR                             {$$ = new AssignOperator($1, $3);}
;
VAR_TYPE:               INT                                         {$$ = new TypeNode(TypeIdentifyer::INT_T);}
;
LOGIC_EXPR:             LOGIC_AND_EXPR
|                       LOGIC_EXPR OR LOGIC_AND_EXPR                {$$ = new OrOperator($1, $3);}
;
LOGIC_AND_EXPR:         LOGIC_CMP_EXPR
|                       LOGIC_AND_EXPR AND LOGIC_CMP_EXPR           {$$ = new AndOperator($1, $3);}
;
LOGIC_CMP_EXPR:         LOGIC_FINAL_EXPR
|                       LOGIC_CMP_EXPR EQ LOGIC_FINAL_EXPR          {$$ = new EqOperator($1, $3);}
|                       LOGIC_CMP_EXPR LESS LOGIC_FINAL_EXPR        {$$ = new LessOperator($1, $3);}
|                       LOGIC_CMP_EXPR GR LOGIC_FINAL_EXPR          {$$ = new GrOperator($1, $3);}
|                       LOGIC_CMP_EXPR LESS_EQ LOGIC_FINAL_EXPR     {$$ = new LessEqOperator($1, $3);}
|                       LOGIC_CMP_EXPR GR_EQ LOGIC_FINAL_EXPR       {$$ = new GrEqOperator($1, $3);}
|                       LOGIC_CMP_EXPR NOT_EQ LOGIC_FINAL_EXPR      {$$ = new NotEqOperator($1, $3);}
;
LOGIC_FINAL_EXPR:       ARITH_EXPR
|                       NOT LOGIC_FINAL_EXPR                        {$$ = new NotOperator($2);}
;
ARITH_EXPR:             ARITH_MUL_EXPR
|                       ARITH_EXPR '+' ARITH_MUL_EXPR               {$$ = new PlusOperator($1, $3);}
|                       ARITH_EXPR '-' ARITH_MUL_EXPR               {$$ = new MinusOperator($1, $3);}
;
ARITH_MUL_EXPR:         ARITH_FINAL_EXPR
|                       ARITH_FINAL_EXPR '*' ARITH_MUL_EXPR         {$$ = new MultOperator($1, $3);}
|                       ARITH_FINAL_EXPR '/' ARITH_MUL_EXPR         {$$ = new DivideOperator($1, $3);}
;
ARITH_FINAL_EXPR:       '(' EXPR ')'                                {$$ = $2}
|                       NUM                                         {$$ = new IntValueNode($1.c_str());}
|                       VAR                                         {$$ = new VariableNode($1);}
|                       RET_FUNCTION_CALL
|                       '-' ARITH_FINAL_EXPR                        {$$ = new UnaryMinusOperator($2);}
;
%%
