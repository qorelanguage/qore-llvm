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
    using R = void*;

    R visit(const qore::ast::IntegerLiteral *e) override {
        QoreValue *qv = new QoreValue();
        *qv = make_int(e->value);
        return qv;
    }
    R visit(const qore::ast::StringLiteral *e) override {
        QoreValue *qv = new QoreValue();
        *qv = make_str(e->value.c_str());
        return qv;
    }
    R visit(const qore::ast::BinaryExpression *e) override {
        QoreValue *l = static_cast<QoreValue*>(e->left->accept(*this));
        QoreValue *r = static_cast<QoreValue*>(e->right->accept(*this));
        QoreValue *result = new QoreValue();

        CLOG("I", "binary: " << *l << ", " << *r);

        *result = eval_add(*l, *r);
        return result;
    }
    R visit(const qore::ast::UnaryExpression *e) override {
        QoreValue *o = static_cast<QoreValue*>(e->operand->accept(*this));
        eval_trim(*o);
        CLOG("I", "unary: " << *o);
        return o;
    }
    R visit(const qore::ast::EmptyStatement *) override {
        return nullptr;
    }
    R visit(const qore::ast::PrintStatement *s) override {
        QoreValue *qv = static_cast<QoreValue*>(s->expression->accept(*this));
        print_qv(*qv);
        return qv;
    }
    R visit(const qore::ast::Program *program) override {
        for (const auto &statement : program->body) {
            statement->accept(*this);
        }
        return nullptr;
    }
};

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
