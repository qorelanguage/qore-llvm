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
    using VariableData = QoreValue *;
    using Value = QoreValue;
    using LValue = QoreValue *;

    VariableData createVariable(const std::string &name) {
        QoreValue *qv = new QoreValue();
        qv->tag = Tag::Nothing;
        return qv;
    }

    void destroyVariable(const VariableData &var) {
        delete var;
    }

    StringLiteralData createStringLiteral(const std::string &value) {
        QoreValue *qv = new QoreValue();
        *qv = make_str(value.c_str());
        return qv;
    }

    void destroyStringLiteral(const StringLiteralData &str) {
        strongDeref(*str);
        delete str;
    }

    Value add(const Value &l, const Value &r) {
        return eval_add(l, r);
    }

    Value assign(const LValue &dest, const Value &newValue) {
        QoreValue originalValue = *dest;
        *dest = newValue;
        return originalValue;
    }

    Value getNothing() {
        QoreValue qv;
        qv.tag = Tag::Nothing;
        return qv;
    }

    void lifetimeEnd(const VariableData &) {
    }


    void lifetimeStart(const VariableData &) {
    }

    Value loadStringLiteralValue(const StringLiteralData &str) {
        return *str;
    }

    Value loadUnique(const LValue &lval) {
        return load_unique(lval);
    }

    Value loadVariableValue(const VariableData &var) {
        return *var;
    }

    LValue loadVarPtr(const VariableData &var) {
        return var;
    }

    void print(const Value &v) {
        print_qv(v);
    }

    void strongDeref(const QoreValue &qv) {
        ::strongDeref(qv);
    }

    void strongRef(const QoreValue &qv) {
        ::strongRef(qv);
    }

    void trim(const Value &v) {
        eval_trim(v);
    }

    void setLocation(const SourceLocation &loc) {
    }
};

void doInterpret(const qil::Script &script) {
    IntBackend ib;
    Runner<IntBackend> runner(script, ib);
    runner.run();
}

} // namespace qore

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
