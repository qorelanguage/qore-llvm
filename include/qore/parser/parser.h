#ifndef INCLUDE_QORE_PARSER_PARSER_H_
#define INCLUDE_QORE_PARSER_PARSER_H_

#include <memory>
#include <string>
#include "qore/ast/ast.h"

std::unique_ptr<Program> parse(const std::string &source);

#endif /* INCLUDE_QORE_PARSER_PARSER_H_ */
