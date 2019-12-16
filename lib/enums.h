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


#endif //INTERPRETER_ENUMS_H
