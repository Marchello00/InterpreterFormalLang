#include <iostream>
#include <syntax_tree.h>
#include <machine.h>
#include <parser.h>
#include <fstream>
#include <interpreter.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (!set_file(argv[1])) {
            std::cerr << "Can't open file " << std::string(argv[1]) << "\n";
            return 0;
        }
    }
//    std::ifstream f_in;
//    if (argc > 2) {
//        f_in.open(argv[2], std::ifstream::in);
//    }
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