#include <iostream>
#include <syntax_tree.h>
#include <machine.h>
#include <parser.h>
#include <fstream>
#include <interpreter.h>

std::string interactive_hello() {
    return "Hello, You are in interaction version of interpreter. Just enter commands like in python interpreter.\n" +
           flex_interpreter.ps1;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (!set_file(argv[1])) {
            std::cerr << "Can't open file " << std::string(argv[1]) << "\n";
            return 0;
        }
    } else {
        std::cout << interactive_hello();
    }
    Interpreter interpreter;
    while (!flex_interpreter.eof) {
        flex_interpreter.atStart = true;
        int status = yyparse(&interpreter);
        if (status) {
            flex_interpreter.atStart = true;
        } else {
            interpreter.interpret();
        }
    }
    return 0;
}