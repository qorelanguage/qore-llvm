//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 Qore Technologies
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//------------------------------------------------------------------------------
///
/// \file
/// \brief Interpreter implementation.
///
//------------------------------------------------------------------------------
#include "qore/in/Interpreter.h"
#include <cassert>
#include <vector>
#include "qore/rt/Func.h"
#include "StatementExecutor.h"
#include "ExpressionEvaluator.h"

namespace qore {
namespace in {

Interpreter::Interpreter() {
}

Interpreter::~Interpreter() {
    for (auto &p : globals) {
        rt::qvalue v = rt::gv_free(p.second);
        if (p.first->getType().rtType == rt::qvalue_type::Ptr) {
            rt::decRef(v.p);
        }
    }
    for (auto &p : strings) {
        rt::decRef(p.second);
    }
}

void Interpreter::addStringLiteral(const ir::StringLiteral &sl) {
    strings[&sl] = rt::createString(sl.getValue().c_str(), sl.getValue().size());
}

void Interpreter::addGlobalVariable(const ir::GlobalVariable &gv) {
    Locals locals;
    ExpressionEvaluator evaluator(*this, locals);
    Value v = evaluator.evaluate(gv.getInitExpression());
    try {
        globals[&gv] = rt::gv_create(v.value);
    } catch (rt::Exception &e) {
        v.cleanup(e);
        throw;
    }
}

void Interpreter::execute(Locals &locals, const ir::Statement &stmt) {
    StatementExecutor executor(*this, locals);
    executor.execute(stmt);
}

rt::GlobalVariable *Interpreter::getGlobalVariableValue(const ir::GlobalVariable &gv) const {
    auto it = globals.find(&gv);
    assert(it != globals.end());
    return it->second;
}

rt::qptr Interpreter::getStringLiteralValue(const ir::StringLiteral &sl) const {
    auto it = strings.find(&sl);
    assert(it != strings.end());
    return it->second;
}

void Interpreter::run(const ir::Script &script, const ir::UserFunction &f) {
    qore::in::Interpreter interpreter;
    for (auto &sl : script.getStringLiterals()) {
        interpreter.addStringLiteral(*sl);
    }
    for (auto &gv : script.getGlobalVariables()) {
        interpreter.addGlobalVariable(*gv);
    }

    Locals locals;

    for (auto &lv : f.getLocalVariables()) {
        locals.add(*lv);
    }

    StatementExecutor executor(interpreter, locals);
    for (auto &stmt : f.getStatements()) {
        executor.execute(*stmt);
    }
    //FIXME cleanup locals when an exception is thrown
    for (auto &lv : f.getLocalVariables()) {
        if (lv->getType().rtType == rt::qvalue_type::Ptr) {
            rt::qvalue v = locals.get(*lv);
            if (v.p) {
                rt::decRef(v.p);
            }
        }
    }
}

} // namespace in
} // namespace qore
