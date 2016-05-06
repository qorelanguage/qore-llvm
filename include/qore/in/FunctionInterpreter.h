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
    explicit FunctionInterpreter(F &f) : f(f) {
    }

    void run(code::Block &bb) {
        const code::Block *b = &bb;
        code::Block::Iterator ins = b->begin();
        while (true) {
            assert(ins != b->end());
            try {
                //FIXME replace with visitor
                switch (ins->getKind()) {
                    case code::Instruction::Kind::Branch: {
                        const code::Branch &i = static_cast<const code::Branch &>(*ins);
                        if (f.getTemp(i.getCondition()).b) {
                            b = &i.getTrueDest();
                        } else {
                            b = &i.getFalseDest();
                        }
                        ins = b->begin();
                        continue;
                    }
                    case code::Instruction::Kind::ConstInt:
                        exec(static_cast<const code::ConstInt &>(*ins));
                        break;
                    case code::Instruction::Kind::ConstString:
                        exec(static_cast<const code::ConstString &>(*ins));
                        break;
                    case code::Instruction::Kind::GlobalGet:
                        exec(static_cast<const code::GlobalGet &>(*ins));
                        break;
                    case code::Instruction::Kind::GlobalInit:
                        exec(static_cast<const code::GlobalInit &>(*ins));
                        break;
                    case code::Instruction::Kind::GlobalReadLock:
                        exec(static_cast<const code::GlobalReadLock &>(*ins));
                        break;
                    case code::Instruction::Kind::GlobalReadUnlock:
                        exec(static_cast<const code::GlobalReadUnlock &>(*ins));
                        break;
                    case code::Instruction::Kind::GlobalSet:
                        exec(static_cast<const code::GlobalSet &>(*ins));
                        break;
                    case code::Instruction::Kind::GlobalWriteLock:
                        exec(static_cast<const code::GlobalWriteLock &>(*ins));
                        break;
                    case code::Instruction::Kind::GlobalWriteUnlock:
                        exec(static_cast<const code::GlobalWriteUnlock &>(*ins));
                        break;
                    case code::Instruction::Kind::InvokeBinaryOperator:
                        exec(static_cast<const code::InvokeBinaryOperator &>(*ins));
                        break;
                    case code::Instruction::Kind::InvokeConversion:
                        exec(static_cast<const code::InvokeConversion &>(*ins));
                        break;
                    case code::Instruction::Kind::Jump: {
                        const code::Jump &ii = static_cast<const code::Jump &>(*ins);
                        b = &ii.getDest();
                        ins = b->begin();
                        continue;
                    }
                    case code::Instruction::Kind::LocalGet:
                        exec(static_cast<const code::LocalGet &>(*ins));
                        break;
                    case code::Instruction::Kind::LocalSet:
                        exec(static_cast<const code::LocalSet &>(*ins));
                        break;
                    case code::Instruction::Kind::RefDec:
                        exec(static_cast<const code::RefDec &>(*ins));
                        break;
                    case code::Instruction::Kind::RefInc:
                        exec(static_cast<const code::RefInc &>(*ins));
                        break;
                    case code::Instruction::Kind::RetVoid:
                        return;
                    default:
                        QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ins->getKind()));
                }
                ++ins;
            } catch (Exception &e) {
                if (!ins->getLpad()) {
                    throw;
                }
                //  set 'e' as the 'current exception'
                //      - it is only used during cleanup (by the refDecNoThrow instruction), i.e. the code between
                //        the start of the lpad block and either rethrow/resume or jump to user's catch block, which
                //        will store it to a user-defined local variable
                b = ins->getLpad();
                ins = b->begin();
            }
        }
    }

private:
    void exec(const code::ConstInt &ins) {
        qvalue v;
        v.i = ins.getValue();
        f.setTemp(ins.getDest(), v);
    }

    void exec(const code::ConstString &ins) {
        qvalue v;
        v.p = ins.getString();
        f.setTemp(ins.getDest(), v);
    }

    void exec(const code::GlobalGet &ins) {
        f.setTemp(ins.getDest(), ins.getGlobalVariable().getValue());
    }

    void exec(const code::GlobalInit &ins) {
        ins.getGlobalVariable().initValue(f.getTemp(ins.getInitValue()));
    }

    void exec(const code::GlobalReadLock &ins) {
        ins.getGlobalVariable().readLock();
    }

    void exec(const code::GlobalReadUnlock &ins) {
        ins.getGlobalVariable().readUnlock();
    }

    void exec(const code::GlobalSet &ins) {
        ins.getGlobalVariable().setValue(f.getTemp(ins.getSrc()));
    }

    void exec(const code::GlobalWriteLock &ins) {
        ins.getGlobalVariable().writeLock();
    }

    void exec(const code::GlobalWriteUnlock &ins) {
        ins.getGlobalVariable().writeUnlock();
    }

    void exec(const code::InvokeBinaryOperator &ins) {
        f.setTemp(ins.getDest(), ins.getOperator().getFunction()(f.getTemp(ins.getLeft()), f.getTemp(ins.getRight())));
    }

    void exec(const code::InvokeConversion &ins) {
        f.setTemp(ins.getDest(), ins.getConversion().getFunction()(f.getTemp(ins.getArg())));
    }

    void exec(const code::LocalGet &ins) {
        f.setTemp(ins.getDest(), f.getLocal(ins.getLocalVariable()));
    }

    void exec(const code::LocalSet &ins) {
        f.setLocal(ins.getLocalVariable(), f.getTemp(ins.getSrc()));
    }

    void exec(const code::RefDec &ins) {
        qvalue v = f.getTemp(ins.getTemp());
        if (v.p) {
            v.p->decRefCount();
            //FIXME if an exception was created, throw it
        }
    }

    void exec(const code::RefInc &ins) {
        qvalue v = f.getTemp(ins.getTemp());
        if (v.p) {
            v.p->incRefCount();
        }
    }

private:
    F &f;
};

} // namespace in
} // namespace qore

#endif // INCLUDE_QORE_IN_FUNCTIONINTERPRETER_H_
