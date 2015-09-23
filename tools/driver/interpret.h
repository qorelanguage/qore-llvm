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

class InterpretRunner : public qil::TerminatorVisitor {

public:
    InterpretRunner(const qil::Script &script) : script(script), machine(backend), bb(script.code.entryBasicBlock) {
        for (const auto &s : script.strings) {
            s->data = backend.createStringLiteral(s->value);
        }
        for (const auto &v : script.variables) {
            v->data = backend.createVariable(v->name);
        }
    }

    ~InterpretRunner() {
        for (const auto &v : script.variables) {
            backend.destroyVariable(static_cast<IntBackend::VariableData>(v->data));
        }
        for (const auto &s : script.strings) {
            backend.destroyStringLiteral(static_cast<IntBackend::StringLiteralData>(s->data));
        }
    }

    void run() {
        while (bb) {
            machine.run(bb);
            bb->terminator->accept(*this);
        }
    }

    void visit(const qil::ConditionalTerminator *t) override {
        IntBackend::Value value = machine.pop();
        machine.checkEmpty();
        bb = eval_cond(value) ? t->thenBlock : t->elseBlock;
        machine.discard(value);
    }
    void visit(const qil::UnconditionalTerminator *t) override {
        machine.checkEmpty();
        bb = t->nextBlock;
    }
    void visit(const qil::RetVoidTerminator *t) override {
        machine.checkEmpty();
        bb = nullptr;
    }

private:
    IntBackend backend;
    const qil::Script &script;
    qil::Machine<IntBackend> machine;
    qil::BasicBlock *bb;
};

void doInterpret(const qil::Script &script) {
    InterpretRunner runner(script);
    runner.run();
}

} // namespace qore

#endif /* TOOLS_DRIVER_INTERPRET_H_ */
