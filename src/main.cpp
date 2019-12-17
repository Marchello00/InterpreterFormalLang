#include <iostream>
#include <syntax_tree.h>
#include <machine.h>
#include <parser.h>
#include <fstream>

int main(int argc, char *argv[]) {
    std::ifstream f_in;
    if (argc > 1) {
        freopen(argv[1], "r", stdin);
    }
    if (argc > 2) {
        f_in.open(argv[2], std::ifstream::in);
    }
    yydebug = 0;
    CmdListNode *root;
    if (yyparse(&root)) {
        return -1;
    }
//    root->print(0, std::cout);
    Machine machine(f_in);
    try {
        root->evaluate(machine);
    } catch  (std::exception &e) {
        std::cout << "Error: " << e.what() << "\n";
    }
    return 0;
}