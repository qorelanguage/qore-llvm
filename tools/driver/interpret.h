#ifndef TOOLS_DRIVER_INTERPRET_H_
#define TOOLS_DRIVER_INTERPRET_H_

#include <cstdlib>
#include "qore/ast/ast.h"
#include "qore/runtime/runtime.h"

class InterpretVisitor : public Visitor {
public:
    R visit(const class IntegerLiteral *e) override {
        QoreValue *qv = new QoreValue();
        *qv = make_int(e->getValue());
        return qv;
    }
    R visit(const class StringLiteral *e) override {
        QoreValue *qv = new QoreValue();
        *qv = make_str(strdup(e->getValue().c_str()));  //TODO strdup
        return qv;
    }
    R visit(const class VariableLoadExpression *) override {
        return nullptr;
    }
    R visit(const class Assignment *) override {
        return nullptr;
    }
    R visit(const class EmptyStatement *) override {
        return nullptr;
    }
    R visit(const class ExpressionStatement *) override {
        return nullptr;
    }
    R visit(const class PrintStatement *s) override {
        QoreValue *qv = static_cast<QoreValue*>(s->getExpression()->accept(*this));
        print_qv(*qv);
        return qv;
    }
    R visit(const class Program *p) override {
        p->forEachStatement([this](const Statement *s){s->accept(*this);});
        return nullptr;
    }
};

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
