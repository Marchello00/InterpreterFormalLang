#ifndef INTERPRETER_INTERPRETER_H
#define INTERPRETER_INTERPRETER_H

#include <iostream>
#include <string>
#include <syntax_tree.h>
#include <machine.h>

struct FlexInterpreter {
    const std::string ps1 = ">>>  "; // prompt to start statement
    const std::string ps2 = "... "; // prompt to continue statement
    bool eof = false; // set by the EOF action in the parser
    bool completeLine = false; // managed by yyread
    bool atStart = false; // true before scanner sees printable chars on line
};

class Interpreter {
public:
    explicit Interpreter(std::istream &in = std::cin,
                         std::ostream &out = std::cout) :
            machine(in, out), node_(nullptr) {}

    void set_node(Node *node) {
        node_ = node;
    }

    void interpret(Node *node) {
        try {
            if (node) {
                node->evaluate(machine);
            }
        } catch (std::exception &e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }

    void interpret() {
        interpret(node_);
    }

private:
    Machine machine;
    Node *node_;
};

#endif //INTERPRETER_INTERPRETER_H
