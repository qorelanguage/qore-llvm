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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IN_FUNCTIONINTERPRETER_H_
#define INCLUDE_QORE_IN_FUNCTIONINTERPRETER_H_

#include <algorithm>
#include <cassert>
#include <vector>
#include "qore/core/Exception.h"
#include "qore/core/code/Block.h"
#include "qore/core/util/Debug.h"

namespace qore {
namespace in {

template<typename F>
class FunctionInterpreter {

public:
    using ReturnType = void;

public:
    FunctionInterpreter(F &f, code::Block &entryBlock) : f(f), done(false), it(entryBlock.begin()) {
    }

    void run() {
        while (!done) {
            const code::Instruction &ins = *it;
            ++it;
            try {
                ins.accept(*this);
            } catch (Exception &e) {
                if (!ins.getLpad()) {
                    throw;
                }
                //  set 'e' as the 'current exception'
                //      - it is only used during cleanup (by the refDecNoThrow instruction), i.e. the code between
                //        the start of the lpad block and either rethrow/resume or jump to user's catch block, which
                //        will store it to a user-defined local variable
                it = ins.getLpad()->begin();
            }
        }
    }

    void visit(const code::Branch &ins) {
        if (f.getTemp(ins.getCondition()).b) {
            it = ins.getTrueDest().begin();
        } else {
            it = ins.getFalseDest().begin();
        }
    }

    void visit(const code::ConstInt &ins) {
        qvalue v;
        v.i = ins.getValue();
        f.setTemp(ins.getDest(), v);
    }

    void visit(const code::ConstString &ins) {
        qvalue v;
        v.p = &ins.getString();
        f.setTemp(ins.getDest(), v);
    }

    void visit(const code::GlobalGet &ins) {
        f.setTemp(ins.getDest(), ins.getGlobalVariable().getValue());
    }

    void visit(const code::GlobalInit &ins) {
        ins.getGlobalVariable().initValue(f.getTemp(ins.getInitValue()));
    }

    void visit(const code::GlobalReadLock &ins) {
        ins.getGlobalVariable().readLock();
    }

    void visit(const code::GlobalReadUnlock &ins) {
        ins.getGlobalVariable().readUnlock();
    }

    void visit(const code::GlobalSet &ins) {
        ins.getGlobalVariable().setValue(f.getTemp(ins.getSrc()));
    }

    void visit(const code::GlobalWriteLock &ins) {
        ins.getGlobalVariable().writeLock();
    }

    void visit(const code::GlobalWriteUnlock &ins) {
        ins.getGlobalVariable().writeUnlock();
    }

    void visit(const code::InvokeBinaryOperator &ins) {
        f.setTemp(ins.getDest(), ins.getOperator().getFunction()(f.getTemp(ins.getLeft()), f.getTemp(ins.getRight())));
    }

    void visit(const code::InvokeConversion &ins) {
        f.setTemp(ins.getDest(), ins.getConversion().getFunction()(f.getTemp(ins.getArg())));
    }

    void visit(const code::Jump &ins) {
        it = ins.getDest().begin();
    }

    void visit(const code::LocalGet &ins) {
        f.setTemp(ins.getDest(), f.getLocal(ins.getLocalVariable()));
    }

    void visit(const code::LocalSet &ins) {
        f.setLocal(ins.getLocalVariable(), f.getTemp(ins.getSrc()));
    }

    void visit(const code::RefDec &ins) {
        qvalue v = f.getTemp(ins.getTemp());
        if (v.p) {
            v.p->decRefCount();
            //FIXME if an exception was created, throw it
        }
    }

    void visit(const code::RefInc &ins) {
        qvalue v = f.getTemp(ins.getTemp());
        if (v.p) {
            v.p->incRefCount();
        }
    }

    void visit(const code::RetVoid &ins) {
        done = true;
    }

    void visit(const code::Instruction &ins) {
        QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ins.getKind()));
    }

private:
    F &f;
    bool done;
    code::Block::Iterator it;
};

} // namespace in
} // namespace qore

#endif // INCLUDE_QORE_IN_FUNCTIONINTERPRETER_H_
