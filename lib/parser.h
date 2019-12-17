#ifndef INTERPRETER_PARSER_H
#define INTERPRETER_PARSER_H

#include <syntax_tree.h>

extern int yydebug;
extern int yyparse(CmdListNode **root);

#endif //INTERPRETER_PARSER_H
