#ifndef TOOLS_DRIVER_INTERPRET_H_
#define TOOLS_DRIVER_INTERPRET_H_

#include <cstdlib>
#include "qore/ast/Program.h"
#include "qore/runtime/runtime.h"

inline std::ostream &operator<<(std::ostream &os, const QoreValue &qv) {
    if (qv.tag == Tag::Int) {
        return os << "intValue(" << qv.intValue << ")";
    } else {
        return os << "strValue(" << qv.strValue << ")";
    }
}

/**
 * \private
 */
class InterpretVisitor : public qore::ast::Visitor {
public:
    QoreValue *currentValue;

    void visit(const qore::ast::IntegerLiteral *e) override {
        currentValue = new QoreValue();
        make_int(currentValue, e->value);
    }
    void visit(const qore::ast::StringLiteral *e) override {
        currentValue = new QoreValue();
        make_str(currentValue, e->value.c_str());
    }
    void visit(const qore::ast::BinaryExpression *e) override {
        e->left->accept(*this);
        QoreValue *l = currentValue;
        e->right->accept(*this);
        QoreValue *r = currentValue;
        currentValue = new QoreValue();

        CLOG("I", "binary: " << *l << ", " << *r);

        eval_add(currentValue, l, r);
        delete l;
        delete r;
    }
    void visit(const qore::ast::UnaryExpression *e) override {
        e->operand->accept(*this);
        eval_trim(currentValue);
        CLOG("I", "unary: " << *currentValue);
    }
    void visit(const qore::ast::EmptyStatement *) override {
    }
    void visit(const qore::ast::PrintStatement *s) override {
        s->expression->accept(*this);
        print_qv(currentValue);
        delete currentValue;
    }
    void visit(const qore::ast::ExpressionStatement *s) override {
        s->expression->accept(*this);
        delete currentValue;
    }
    void visit(const qore::ast::Program *program) override {
        for (const auto &statement : program->body) {
            statement->accept(*this);
        }
    }
};

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
