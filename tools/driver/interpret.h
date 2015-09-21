#ifndef TOOLS_DRIVER_INTERPRET_H_
#define TOOLS_DRIVER_INTERPRET_H_

#include <cstdlib>
#include <map>
#include <string>
#include "qore/ast/Program.h"
#include "qore/runtime/runtime.h"
#include "qore/common/Util.h"


namespace qore {

class IntBackend {

public:
    using StringLiteralData = QoreValue *;
    using LocalVariableData = QoreValue *;
    using Value = QoreValue;
    using LValue = QoreValue *;

    LocalVariableData createLocalVariable(const std::string &name) {
        QoreValue *qv = new QoreValue();
        qv->tag = Tag::Nothing;
        return qv;
    }

    StringLiteralData createStringLiteral(const std::string &value) {
        QoreValue *qv = new QoreValue();
        *qv = make_str(value.c_str());
        return qv;
    }

    void destroy(QoreValue *qv) {
        strongDeref(*qv);
        delete qv;
    }

    void destroy(QoreValue &qv) {
        strongDeref(qv);
        qv.tag = Tag::Nothing;
    }

    Value load(QoreValue *qv) {
        strongRef(*qv);
        return *qv;
    }

    LValue loadPtr(LocalVariableData var) {
        return var;
    }

    Value loadUnique(LValue lval) {
        return load_unique(lval);
    }

    void swap(LValue v1, Value &v2) {
        std::swap(*v1, v2);
    }

    void trim(Value &v) {
        eval_trim(v);
    }

    Value add(const Value &l, const Value &r) {
        return eval_add(l, r);
    }

    void print(const Value &v) {
        print_qv(v);
    }
};

void doInterpret(const analyzer::Script &script) {
    IntBackend ib;
    Runner<IntBackend> runner(script, ib);
    runner.run();
}
}

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
