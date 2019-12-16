#include <iostream>
#include <syntax_tree.h>

extern int yydebug;
extern int yyparse(CmdListNode **root);

int main(int argc, char *argv[]) {
    if (argc > 1) {
        freopen(argv[1], "r", stdin);
    }
    yydebug = 0;
    CmdListNode *root;
    if (yyparse(&root)) {
        return -1;
    }
    root->print(0, std::cout);
    return 0;
}