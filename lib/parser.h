#ifndef INTERPRETER_PARSER_H
#define INTERPRETER_PARSER_H

#include <interpreter.h>

extern FlexInterpreter flex_interpreter;
extern bool set_file(char *filename);
extern int yydebug;
extern int yyparse(Interpreter *root);

typedef struct {
    std::string str;
    ExpressionNode *expr;
    CmdNode *cmd;
    CmdListNode *cmd_list;
    OperatorNode  *oper;
    TypeNode *var_type;
} YYSTYPE_struct;

#endif //INTERPRETER_PARSER_H
