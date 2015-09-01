#ifndef TOOLS_DUMP_H_
#define TOOLS_DUMP_H_

#include <iostream>
#include "qore/ast/ast.h"

class DumpVisitor : public Visitor {

public:
    DumpVisitor() {
    }

    R visit(const class IntegerLiteral *e) override {
        std::cout << "IntegerLiteral " << e->getValue() << std::endl;
        return nullptr;
    }

    R visit(const class StringLiteral *e) override {
        std::cout << "StringLiteral \"" << e->getValue() << '"' << std::endl;
        return nullptr;
    }

    R visit(const class BinaryExpression *e) override {
        std::cout << "BinaryExpression {" << std::endl;
        ++indent;
        std::cout << i() << "left: ";
        e->getLeft()->accept(*this);
        std::cout << i() << "right: ";
        e->getRight()->accept(*this);
        --indent;
        std::cout << i() << "}" << std::endl;
        return nullptr;
    }

    R visit(const class EmptyStatement *) override {
        std::cout << i() << "EmptyStatement" << std::endl;
        return nullptr;
    }

    R visit(const class PrintStatement *s) override {
        std::cout << i() << "PrintStatement: ";
        s->getExpression()->accept(*this);
        return nullptr;
    }

    R visit(const class Program *program) override {
        std::cout << "Program {" << std::endl;
        ++indent;
        program->forEachStatement([this](const Statement *s){ s->accept(*this); });
        std::cout << "}" << std::endl;
        return nullptr;
    }

private:
    std::string i() {
        return std::string(2*indent, ' ');
    }

private:
    int indent{0};
};

#endif /* TOOLS_DUMP_H_ */
