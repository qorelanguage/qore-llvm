#ifndef TOOLS_DRIVER_INTERPRET_H_
#define TOOLS_DRIVER_INTERPRET_H_

#include <cstdlib>
#include <map>
#include <string>
#include "qore/ast/Program.h"
#include "qore/runtime/runtime.h"
#include "qore/common/Util.h"

namespace qore {

/**
 * \brief Writes information about a variable to an output stream.
 * \param os the output stream
 * \param var the variable
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &os, const ast::Variable &var) {
    return os << var.name << "@" << var.getRange().start;
}

inline std::ostream &operator<<(std::ostream &os, const ast::Variable *var) {
    return os << *var;
}

class Value {

public:
    Value() : hasValue(false) {     //TODO hasValue is useful only for logging
        qv.tag = Tag::Nothing;
    }

    ~Value() {
        if (hasValue) {
            LOG("Deref " << qv);
            strongDeref(qv);
        }
    }

    Value(const QoreValue &qv) : qv(qv), hasValue(true) {
        strongRef(qv);
        LOG("Ref " << qv);
    }

    Value(Value &&src) : qv(src.qv), hasValue(src.hasValue) {
        src.qv.tag = Tag::Nothing;
        src.hasValue = false;
    }

    Value &operator=(Value &&src) {
        assert(!hasValue);
        if (src.hasValue) {
            hasValue = true;
            qv = src.qv;
            src.qv.tag = Tag::Nothing;
            src.hasValue = false;
        }
        return *this;
    }

    void swapValue(QoreValue &v) {
        assert(hasValue);
        std::swap(qv, v);
    }

    void print() {
        assert(hasValue);
        LOG("Print " << qv);
        print_qv(qv);
    }

    bool evalCond() {
        assert(hasValue);
        LOG("EvalCond " << qv);
        return eval_cond(qv);
    }

    Value add(Value &right) {
        return Value(eval_add(qv, right.qv), false);        //TODO all refs and derefs are handled here, so runtime should not care -> make eval_add return ref = 0 ?
    }

    Value dup() {
        return Value(qv);
    }

    void trim() {       //assumes unique
        assert(hasValue);
        LOG("Trim " << qv);
        eval_trim(qv);
    }


private:
    Value(const QoreValue &qv, bool claimRef) : qv(qv), hasValue(true) {
        if (claimRef) {
            strongRef(qv);
            LOG("Ref " << qv);
        }
    }

private:
    Value(const Value &) = delete;
    Value &operator=(const Value &) = delete;

private:
    QoreValue qv;
    bool hasValue;

    friend std::ostream &operator<<(std::ostream &os, const Value &value) {
        return os << value.qv;
    }

    friend class LValue;
};

class LValue {

public:
    LValue(ast::Variable *var = nullptr) : var(var) {
        if (var) {
            LOG("Acquire lock: " << *var);
        }
    }

    ~LValue() {
        if (var) {
            LOG("Release lock: " << *var);
        }
    }

    LValue(LValue &&src) : var(src.var) {
        src.var = nullptr;
    }

    LValue &operator=(LValue &&src) {
        var = src.var;
        src.var = nullptr;
        return *this;
    }

    void swapValue(Value &qv) {
        assert(var);
        QoreValue *v = static_cast<QoreValue *>(var->data);
        LOG("Assign " << qv << " to " << var);
        qv.swapValue(*v);
    }

    Value loadUnique() {
        QoreValue *v = static_cast<QoreValue *>(var->data);
        LOG("Load unique " << *v);
        return Value(load_unique(v), false);
    }

private:
    LValue(const LValue &) = delete;
    LValue &operator=(const LValue &) = delete;

private:
    ast::Variable *var;
};

class LValueEvaluator : private ast::ExpressionVisitor {
private:
    LValue result;

public:
    LValueEvaluator() {
    }

    static LValue eval(ast::Expression::Ptr &node) {
        LValueEvaluator v;
        node->accept(v);
        return std::move(v.result);
    }

private:
    void visit(ast::IntegerLiteral::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::StringLiteral::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::BinaryExpression::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::UnaryExpression::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::Assignment::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::VarDecl::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::Identifier::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::StringConstant::Ptr node) override {QORE_UNREACHABLE("Not implemented");}

    void visit(ast::Variable::Ptr node) override {
        result = LValue(node.get());
    }
};

class ValueEvaluator : private ast::ExpressionVisitor {
private:
    Value curVal;

public:
    ValueEvaluator() {
    }

    static Value eval(ast::Expression::Ptr &node) {
        ValueEvaluator v;
        node->accept(v);
        return std::move(v.curVal);
    }

private:
    void visit(ast::IntegerLiteral::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::StringLiteral::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::VarDecl::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::Identifier::Ptr node) override {QORE_UNREACHABLE("Not implemented");}

    void visit(ast::UnaryExpression::Ptr node) override {
        Value val;
        {
            LValue lval = LValueEvaluator::eval(node->operand);
            val = lval.loadUnique();
            val.trim();
            curVal = val.dup();         //if needs value
            lval.swapValue(val);
            //release lock
        }
    }

    void visit(ast::BinaryExpression::Ptr node) override {
        Value left = eval(node->left);
        Value right = eval(node->right);
        curVal = left.add(right);
    }


    void visit(ast::Variable::Ptr node) override {
        curVal = Value(*static_cast<QoreValue *>(node->data));
    }

    void visit(ast::StringConstant::Ptr node) override {
        curVal = Value(*static_cast<QoreValue *>(node->data));
    }

    void visit(ast::Assignment::Ptr node) override {
        Value right = eval(node->right);
        {
            LValue lval = LValueEvaluator::eval(node->left);
            //type conversion
            curVal = right.dup();   //if needs value
            lval.swapValue(right);
            //release lock
        }
    }
};

class InterpretVisitor : public ast::StatementVisitor {

public:
    void visit(ast::EmptyStatement::Ptr node) override {QORE_UNREACHABLE("Not implemented");}

    void visit(ast::TryStatement::Ptr node) override {
        try {
            node->tryBody->accept(*this);
        } catch (...) {
            node->catchBody->accept(*this);
        }
    }

    void visit(ast::IfStatement::Ptr node) override {
        if (ValueEvaluator::eval(node->condition).evalCond()) {
            node->thenBranch->accept(*this);
        } else {
            node->elseBranch->accept(*this);
        }
    }

    void visit(ast::ExpressionStatement::Ptr node) override {
        ValueEvaluator::eval(node->expression);
    }

    void visit(ast::PrintStatement::Ptr node) override {
        ValueEvaluator::eval(node->expression).print();
    }

    void visit(ast::CompoundStatement::Ptr node) override {
        for (auto &stmt : node->statements) {
            stmt->accept(*this);
        }
    }

    void visit(ast::ScopedStatement::Ptr node) override {
        for (ast::Variable::Ptr &v : node->variables) {
            LOG("Lifetime start: " << *v);
        }
        try {
            node->statement->accept(*this);
        } catch (...) {
            for (auto it = node->variables.rbegin(); it != node->variables.rend(); ++it) {
                LOG("Lifetime end: " << **it);
                QoreValue *qv = static_cast<QoreValue *>((*it)->data);
                strongDeref(*qv);
                qv->tag = Tag::Nothing;
            }
            throw;
        }
        for (auto it = node->variables.rbegin(); it != node->variables.rend(); ++it) {
            LOG("Lifetime end: " << **it);
            QoreValue *qv = static_cast<QoreValue *>((*it)->data);
            strongDeref(*qv);
            qv->tag = Tag::Nothing;
        }
    }
};

class Interpreter {

public:
    Interpreter(qil::Script &script) : script(script) {
        for (auto &s : script.strings) {
            QoreValue *qv = new QoreValue();
            *qv = make_str(s->value.c_str());
            s->data = qv;
        }
        for (auto &v : script.variables) {
            QoreValue *qv = new QoreValue();
            qv->tag = Tag::Nothing;
            v->data = qv;
        }
    }

    ~Interpreter() {
        for (auto &v : script.variables) {
            QoreValue *qv = static_cast<QoreValue *>(v->data);
            strongDeref(*qv);   //shouldn't be needed, at this point all variables must be out of scope
            delete qv;
        }
        for (auto &s : script.strings) {
            QoreValue *qv = static_cast<QoreValue *>(s->data);
            strongDeref(*qv);
            delete qv;
        }
    }

    void run() {
        try {
            InterpretVisitor v;
            script.body->accept(v);
        } catch (...) {
            LOG("EXCEPTION CAUGHT");
        }
    }

private:
    qil::Script &script;
};

void doInterpret(qil::Script &script) {
    Interpreter i(script);
    i.run();
}

} // namespace qore

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
