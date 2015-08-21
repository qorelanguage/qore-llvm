#include <iostream>
#include "qore/common/common.h"
#include "parser.hpp"

void yyerror(const char *msg) {
    printf("Error: %s\n", msg);
}

void yy_scan_string(const char *yy_str);

std::unique_ptr<Program> ROOT;

int main() {
    yy_scan_string("a=\"x\";b=8;c=a;a=\"y\";c=b=2;");
    yyparse();
    ROOT->dump();
    return 0;
}
