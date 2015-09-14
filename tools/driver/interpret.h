#ifndef TOOLS_DRIVER_INTERPRET_H_
#define TOOLS_DRIVER_INTERPRET_H_

#include <cstdlib>
#include <map>
#include <string>
#include "qore/ast/Program.h"
#include "qore/runtime/runtime.h"

struct Interpreter : public qore::FunctionProcessor {
    Interpreter() {
    }

    void takeOwnership(qore::Storage *s) override {
        objects.emplace_back(s);
    }

    void processAction(const qore::Action &a) override {
        switch (a.type) {
            case qore::Action::Add:
                eval_add(V(a.s1), *V(a.s2), *V(a.s3));
                break;
            case qore::Action::Assign:
                eval_assign(V(a.s1), *V(a.s2));
                break;
            case qore::Action::LifetimeStart:
                a.s1->tag = new QoreValue();
                make_nothing(V(a.s1));
                if (a.s1->isConstant) {
                    make_str(V(a.s1), static_cast<const qore::Constant*>(a.s1)->getValue().c_str());
                }
                break;
            case qore::Action::LifetimeEnd:
                delete V(a.s1);
                break;
            case qore::Action::Print:
                print_qv(*V(a.s1));
                break;
            case qore::Action::Return:
                break;
            case qore::Action::Trim:
                eval_trim(V(a.s1));
                break;
            default:
                QORE_UNREACHABLE("NOT IMPLEMENTED: " << a);
        }
    }

    static inline QoreValue *V(const qore::Storage *s) {
        return static_cast<QoreValue*>(s->tag);
    }

    std::vector<std::unique_ptr<qore::Storage>> objects;
};

void doInterpret(const std::unique_ptr<qore::Function> &f) {
    Interpreter interpreter;
    for (const auto &a : f->actions) {
        interpreter.processAction(a);
    }
    //note: does not call interpreter.takeOwnership, because those are owned by f
}

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
