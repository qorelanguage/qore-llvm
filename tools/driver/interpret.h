#ifndef TOOLS_DRIVER_INTERPRET_H_
#define TOOLS_DRIVER_INTERPRET_H_

#include <cstdlib>
#include <map>
#include <string>
#include "qore/ast/Program.h"
#include "qore/runtime/runtime.h"

#define QORE_LOG_COMPONENT "INTERPRET"

/**
 * \private
 */
class InterpretVisitor : public qore::ast::Visitor {
public:
    QoreValue currentValue;

    void visit(const qore::ast::IntegerLiteral *e) override {
        make_int(&currentValue, e->value);
    }
    void visit(const qore::ast::StringLiteral *e) override {
        make_str(&currentValue, e->value.c_str());
    }
    void visit(const qore::ast::BinaryExpression *e) override {
        e->left->accept(*this);
        QoreValue l = currentValue;
        e->right->accept(*this);
        QoreValue r = currentValue;
        eval_add(&currentValue, &l, &r);
    }
    void visit(const qore::ast::UnaryExpression *e) override {
        e->operand->accept(*this);
        eval_trim(&currentValue);
    }
    void visit(const qore::ast::Assignment *node) override {
        node->left->accept(*this);
        QoreValue l = currentValue;
        node->right->accept(*this);
        eval_assign(&l, &currentValue);     //will be dereferenced at the end of the expression / statement
    }
    void visit(const qore::ast::VarDecl *node) override {
        auto v = new QoreValue();
        make_nothing(v);
        variables[node->name] = v;
        make_lvalue(&currentValue, v);
    }
    void visit(const qore::ast::Identifier *node) override {
        LOG_FUNCTION();
        make_lvalue(&currentValue, variables[node->name]);
    }
    void visit(const qore::ast::EmptyStatement *) override {
    }
    void visit(const qore::ast::PrintStatement *s) override {
        s->expression->accept(*this);
        print_qv(&currentValue);
        deref(&currentValue);
    }
    void visit(const qore::ast::ExpressionStatement *s) override {
        LOG_FUNCTION();
        s->expression->accept(*this);
        deref(&currentValue);
    }
    void visit(const qore::ast::Program *program) override {
        LOG_FUNCTION();
        for (const auto &statement : program->body) {
            statement->accept(*this);
        }
        for (auto &p : variables) {
            QoreValue *qv = p.second;
            deref(qv);
            delete qv;
        }
    }

private:
    std::map<std::string, QoreValue *> variables;
};

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
