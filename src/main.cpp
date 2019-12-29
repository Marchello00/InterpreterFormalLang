#include <iostream>
#include <fstream>
#include <syntax_tree.h>
#include <machine.h>
#include <parser.h>
#include <interpreter.h>

std::string interactive_hello() {
    return "Hello, You are in interaction version of interpreter. Just enter commands like in python interpreter.\n" +
           flex_interpreter.ps1;
}

void err_file(const char *filename) {
    std::cerr << "Can't open file " << std::string(filename) << "\n";
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (!set_file(argv[1])) {
            err_file(argv[1]);
            return 0;
        }
    } else {
        std::cout << interactive_hello();
    }
    bool input_file_fl = false;
    std::ifstream fin;
    if (argc > 2) {
        input_file_fl =  true;
        fin.open(argv[2]);
        if (!fin) {
            err_file(argv[2]);
            return 0;
        }
    }
    Interpreter interpreter((input_file_fl ? fin : std::cin));
    while (!flex_interpreter.eof) {
        flex_interpreter.atStart = true;
        int status = yyparse(&interpreter);
        if (status) {
            flex_interpreter.atStart = true;
        }
    }
    return 0;
}