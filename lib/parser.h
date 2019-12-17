#ifndef INTERPRETER_PARSER_H
#define INTERPRETER_PARSER_H

#include <interpreter.h>

extern FlexInterpreter flex_interpreter;
extern bool set_file(char *filename);
extern int yydebug;
extern int yyparse(Interpreter *root);

#endif //INTERPRETER_PARSER_H
