#ifndef TOOLS_DRIVER_INTERPRET_H_
#define TOOLS_DRIVER_INTERPRET_H_

#include <cstdlib>
#include <map>
#include <string>
#include "qore/ast/Program.h"
#include "qore/runtime/runtime.h"

static inline QoreValue *V(const qore::Storage *s) {
    return static_cast<QoreValue*>(s->tag);
}

void doInterpret(const std::unique_ptr<qore::Function> &f) {
    for (const auto &a : f->actions) {
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
            case qore::Action::LoadValue:
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
}

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
