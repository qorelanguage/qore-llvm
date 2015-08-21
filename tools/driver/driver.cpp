#include <iostream>
#include "qore/common/common.h"
#include "parser.hpp"

void yyerror(const char *msg) {
    printf("Error: %s\n", msg);
}

int main() {
    std::cout << "test(6) = " << test(6) << std::endl;
    yyparse();
    return 0;
}
