#include "qore/parser/parser.h"
#include "parser.hpp"

std::unique_ptr<Program> ROOT;

void yy_scan_string(const char *yy_str);

void yyerror(const char *msg) {
    printf("Error: %s\n", msg);
}

std::unique_ptr<Program> parse(const std::string &source) {
    yy_scan_string(source.c_str());
    yyparse();
    return std::move(ROOT);
}
