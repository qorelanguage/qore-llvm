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
class InterpretVisitor : public qore::ast::ExpressionVisitor, public qore::ast::StatementVisitor, public qore::ast::ProgramVisitor {
public:
    QoreValue *current;
    std::vector<QoreValue*> temps;

    void visit(const qore::ast::IntegerLiteral *e) override {
        LOG_FUNCTION();
        current = new QoreValue();
        temps.push_back(current);
        make_int(current, e->value);
    }
    void visit(const qore::ast::StringLiteral *e) override {
        LOG_FUNCTION();
        current = new QoreValue();
        temps.push_back(current);
        make_str(current, e->value.c_str());
    }
    void visit(const qore::ast::BinaryExpression *e) override {
        LOG_FUNCTION();
        e->left->accept(*this);
        QoreValue *l = current;
        e->right->accept(*this);
        QoreValue *r = current;

        current = new QoreValue();
        eval_add(current, *l, *r);
        freeTemps();
        temps.push_back(current);
    }
    void visit(const qore::ast::UnaryExpression *e) override {
        LOG_FUNCTION();
        e->operand->accept(*this);
        eval_trim(current);
        freeTemps();
    }
    void visit(const qore::ast::Assignment *node) override {
        LOG_FUNCTION();

        node->right->accept(*this);
        QoreValue *value = current;

        node->left->accept(*this);
        eval_assign(current, *value);
        freeTemps();
    }
    void visit(const qore::ast::VarDecl *node) override {
        LOG_FUNCTION();
        current = new QoreValue();
        variables[node->name] = current;
    }
    void visit(const qore::ast::Identifier *node) override {
        LOG_FUNCTION();
        current = variables[node->name];
    }
    void visit(const qore::ast::EmptyStatement *) override {
    }
    void visit(const qore::ast::PrintStatement *s) override {
        LOG_FUNCTION();
        s->expression->accept(*this);
        print_qv(*current);
        freeTemps();
    }
    void visit(const qore::ast::ExpressionStatement *s) override {
        LOG_FUNCTION();
        s->expression->accept(*this);
        freeTemps();
    }
    void visit(const qore::ast::Program *program) override {
        LOG_FUNCTION();
        for (const auto &statement : program->body) {
            statement->accept(*this);
        }
        for (auto &p : variables) {
            delete p.second;
        }
    }

private:
    std::map<std::string, QoreValue *> variables;

private:
    void freeTemps() {
        for (auto &p : temps) {
            delete p;
        }
        temps.clear();
    }
};

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
