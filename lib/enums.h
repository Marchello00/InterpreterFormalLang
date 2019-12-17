#pragma once
#ifndef INTERPRETER_ENUMS_H
#define INTERPRETER_ENUMS_H

enum class NodeType {
    OPERATOR,
    COMMAND,
    COMMAND_LIST,
    CONSTANT,
    VARIABLE,
    TYPE,
};

enum class TypeIdentifyer {
    INT_T,
};

const std::string NOT_INT = "Not int in int expression";
const std::string NOT_BOOL_INT = "Not int and not boolean in bool expression";
const std::string VAR_NEQ_EXPR = "Variable and expression types are different";

#endif //INTERPRETER_ENUMS_H
