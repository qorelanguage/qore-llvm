#ifndef TOOLS_DUMP_H_
#define TOOLS_DUMP_H_

#include <iostream>

class DumpVisitor : public Visitor {

public:
    R visit(const class IntegerLiteral *e) override {
        std::cout << "IntegerLiteral " << e->getValue() << std::endl;
        return nullptr;
    }

    R visit(const class StringLiteral *e) override {
        std::cout << "StringLiteral \"" << e->getValue() << '"' << std::endl;
        return nullptr;
    }

    R visit(const class VariableLoadExpression *e) override {
        std::cout << "VariableLoadExpression " << e->getName() << std::endl;
        return nullptr;
    }

    R visit(const class Assignment *e) override {
        std::cout << "Assignment {" << std::endl;
        ++indent;
        std::cout << i() << "varName: " << e->getVarName() << std::endl;
        std::cout << i() << "value: ";
        e->getValue()->accept(*this);
        --indent;
        std::cout << i() << "}" << std::endl;
        return nullptr;
    }

    R visit(const class ExpressionStatement *s) override {
        std::cout << i() << "ExpressionStatement: ";
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
