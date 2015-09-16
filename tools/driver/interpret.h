#ifndef TOOLS_DRIVER_INTERPRET_H_
#define TOOLS_DRIVER_INTERPRET_H_

#include <cstdlib>
#include <map>
#include <string>
#include "qore/ast/Program.h"
#include "qore/runtime/runtime.h"
#include "qore/common/Util.h"

/**
 * \private
 */
struct Interpreter {
    using Value = std::shared_ptr<QoreValue>;
    using LValue = std::shared_ptr<QoreValue>;

    Interpreter() {
    }

    Value createStringConstant(const std::string &value, const qore::SourceRange &range) {
        QoreValue *qv = new QoreValue();
        make_str(qv, value.c_str());
        return createObject(qv);
    }
    LValue createLocalVariable(const std::string &name, const qore::SourceRange &range) {
        return createObject(new QoreValue());
    }
    LValue createTemporaryVariable(const qore::SourceRange &range) {
        return createObject(new QoreValue());
    }
    void add(LValue dest, Value left, Value right) {
        eval_add(dest.get(), *left, *right);
    }
    void assign(LValue left, Value right) {
        eval_assign(left.get(), *right);
    }
    void trim(LValue lValue) {
        eval_trim(lValue.get());
    }
    void print(Value value) {
        print_qv(*value);
    }
    void finalize() {
    }

private:
    std::shared_ptr<QoreValue> createObject(QoreValue *qv) {
        return std::shared_ptr<QoreValue>(qv);
    }
};

inline QoreValue *V(const qore::il::AValue *v) {
    return static_cast<QoreValue*>(v->tag);
}

void doInterpret(const std::unique_ptr<qore::il::Function> &f) {
    std::vector<std::unique_ptr<QoreValue>> objects;

    for (const auto &c : f->constants) {
        QoreValue *qv = new QoreValue();
        make_str(qv, c->getValue().c_str());
        objects.emplace_back(qv);
        c->tag = qv;
    }
    for (const auto &a : f->actions) {
        switch (a.type) {
            case qore::il::Action::Type::Add: {
                eval_add(V(a.s1), *V(a.s2), *V(a.s3));
                break;
            }
            case qore::il::Action::Type::Assign: {
                eval_assign(V(a.s1), *V(a.s2));
                break;
            }
            case qore::il::Action::Type::LifetimeStart: {
                QoreValue *qv = new QoreValue();
                make_nothing(qv);
//                objects.emplace_back(qv);
                a.s1->tag = qv;
                break;
            }
            case qore::il::Action::Type::LifetimeEnd: {
                delete V(a.s1);
                break;
            }
            case qore::il::Action::Type::Print: {
                print_qv(*V(a.s1));
                break;
            }
            case qore::il::Action::Type::Return: {
                break;
            }
            case qore::il::Action::Type::Trim: {
                eval_trim(V(a.s1));
                break;
            }
            default:
                QORE_UNREACHABLE("NOT IMPLEMENTED: " << a);
        }
    }
}

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
