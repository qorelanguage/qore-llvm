#include <iostream>
#include "qore/common/common.h"
#include "parser.hpp"
#include "dump.h"
#include "gen.h"

void yyerror(const char *msg) {
    printf("Error: %s\n", msg);
}

void yy_scan_string(const char *yy_str);

std::unique_ptr<Program> ROOT;

int main() {
    yy_scan_string("a=5;c=b=a;");//a=\"x\";b=8;c=a;a=\"y\";c=b=2;");
    yyparse();

    std::cout << "Program:" << std::endl;
    ROOT->forEachStatement([](const Statement *s){ std::cout << "  " << s << std::endl; });

    CodeGenVisitor cgv;
    llvm::Module *m = static_cast<llvm::Module*>(ROOT->accept(cgv));
    m->dump();
    delete m;

    return 0;
}
