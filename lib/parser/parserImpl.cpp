#include "qore/parser/parser.h"
#include "parser.hpp"

std::unique_ptr<Program> ROOT;
bool lexNlIsEof;

void yy_scan_string(const char *yy_str);

void yyerror(const char *msg) {
    printf("Error: %s\n", msg);
}

std::unique_ptr<Program> parse(const std::string &source) {
    lexNlIsEof = false;
    yy_scan_string(source.c_str());
    if (yyparse()) {
        return nullptr;
    }
    return std::move(ROOT);
}

std::unique_ptr<Statement> parseStatement() {
    lexNlIsEof = true;
    if (yyparse()) {
        return nullptr;
    }
    return ROOT->getSingleStatement();
}
