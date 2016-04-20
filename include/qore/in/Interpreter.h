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
/// \brief Interpreter definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IN_INTERPRETER_H_
#define INCLUDE_QORE_IN_INTERPRETER_H_

#include <unordered_map>
#include <vector>
#include "qore/rt/Types.h"
#include "qore/rt/Context.h"
#include "qore/in/FunctionInterpreter.h"
#include "qore/comp/as/Script.h"

namespace qore {
namespace in {

class X {

public:
    explicit X(comp::as::Function &f) : f(f), temps(f.getTempCount()), locals(f.getLocalCount()) {
    }

    void setLocal(comp::as::LocalSlot slot, rt::qvalue value) {
        locals[slot.getIndex()] = value;
    }

    rt::qvalue getLocal(comp::as::LocalSlot slot) {
        return locals[slot.getIndex()];
    }

    void setTemp(comp::as::Temp temp, rt::qvalue value) {
        temps[temp.getIndex()] = value;
    }

    rt::qvalue getTemp(comp::as::Temp temp) {
        return temps[temp.getIndex()];
    }

    comp::as::Block &getBlock(Id id) {
        return f.getBlock(id);
    }

private:
    comp::as::Function &f;
    std::vector<rt::qvalue> temps;
    std::vector<rt::qvalue> locals;
};

class Interpreter {

public:
    static void interpret(comp::as::Script &script) {
        rt::Context rtCtx;
        run(rtCtx, script.getInit());
        run(rtCtx, script.getMain());
    }

private:
    Interpreter() {
    }

    static void run(rt::Context &ctx, comp::as::Function &f) {
        X x(f);
        FunctionInterpreter<X> fi(ctx, x);
        fi.run(x.getBlock(0));
    }
};

} // namespace in
} // namespace qore

#endif // INCLUDE_QORE_IN_INTERPRETER_H_
