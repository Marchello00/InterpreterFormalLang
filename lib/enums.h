#ifndef INTERPRETER_ENUMS_H
#define INTERPRETER_ENUMS_H

#include <string>

enum class NodeType {
    OPERATOR,
    COMMAND,
    COMMAND_LIST,
    CONSTANT,
    VARIABLE,
    TYPE,
    EMPTY,
};

enum class TypeIdentifyer {
    INT_T,
    STRING_T,
};

const std::string NOT_INT = "Not int in int expression";
const std::string NOT_BOOL_INT = "Not int and not boolean in bool expression";
const std::string VAR_NEQ_EXPR = "Variable and expression types are different";
const std::string CANT_READ =  "Can't read required type!";

#endif //INTERPRETER_ENUMS_H
