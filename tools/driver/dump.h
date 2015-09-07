#ifndef TOOLS_DUMP_H_
#define TOOLS_DUMP_H_

#include <iostream>
#include "qore/ast/Program.h"

/**
 * \private
 */
class DumpVisitor : public qore::ast::Visitor {

public:
    using R = void*;

    DumpVisitor() {
    }

    R visit(const qore::ast::IntegerLiteral *e) override {
        std::cout << "IntegerLiteral " << e->value << std::endl;
        return nullptr;
    }

    R visit(const qore::ast::StringLiteral *e) override {
        std::cout << "StringLiteral \"" << e->value << '"' << std::endl;
        return nullptr;
    }

    R visit(const qore::ast::BinaryExpression *e) override {
        std::cout << "BinaryExpression {" << std::endl;
        ++indent;
        std::cout << i() << "left: ";
        e->left->accept(*this);
        std::cout << i() << "right: ";
        e->right->accept(*this);
        --indent;
        std::cout << i() << "}" << std::endl;
        return nullptr;
    }

    R visit(const qore::ast::EmptyStatement *) override {
        std::cout << i() << "EmptyStatement" << std::endl;
        return nullptr;
    }

    R visit(const qore::ast::PrintStatement *s) override {
        std::cout << i() << "PrintStatement: ";
        s->expression->accept(*this);
        return nullptr;
    }

    R visit(const qore::ast::Program *program) override {
        std::cout << "Program {" << std::endl;
        ++indent;
        for (const auto &statement : program->body) {
            statement->accept(*this);
        }
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
