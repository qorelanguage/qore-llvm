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
#include "qore/core/util/Debug.h"
#include "qore/comp/as/is.h"

namespace qore {
namespace in {

template<typename F>
class FunctionInterpreter {

public:
    explicit FunctionInterpreter(F &f) : f(f) {
    }

    void run(comp::as::Block &bb) {
        const comp::as::Block *b = &bb;
        comp::as::Block::Iterator ins = b->begin();
        while (true) {
            assert(ins != b->end());
            ++ins;
            try {
                switch (ins->getKind()) {
                    case comp::as::Instruction::Kind::IntConstant:
                        exec(static_cast<const comp::as::IntConstant &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::GetLocal:
                        exec(static_cast<const comp::as::GetLocal &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::SetLocal:
                        exec(static_cast<const comp::as::SetLocal &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::LoadString:
                        exec(static_cast<const comp::as::LoadString &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::RefInc:
                        exec(static_cast<const comp::as::RefInc &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::RefDec:
                        exec(static_cast<const comp::as::RefDec &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::ReadLockGlobal:
                        exec(static_cast<const comp::as::ReadLockGlobal &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::ReadUnlockGlobal:
                        exec(static_cast<const comp::as::ReadUnlockGlobal &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::WriteLockGlobal:
                        exec(static_cast<const comp::as::WriteLockGlobal &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::WriteUnlockGlobal:
                        exec(static_cast<const comp::as::WriteUnlockGlobal &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::GetGlobal:
                        exec(static_cast<const comp::as::GetGlobal &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::SetGlobal:
                        exec(static_cast<const comp::as::SetGlobal &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::MakeGlobal:
                        exec(static_cast<const comp::as::MakeGlobal &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::BinaryOperator:
                        exec(static_cast<const comp::as::BinaryOperator &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::Conversion:
                        exec(static_cast<const comp::as::Conversion &>(*ins));
                        break;
                    case comp::as::Instruction::Kind::RetVoid:
                        return;
                    case comp::as::Instruction::Kind::Jump: {
                        const comp::as::Jump &ii = static_cast<const comp::as::Jump &>(*ins);
                        b = &ii.getDest();
                        ins = b->begin();
                        break;
                    }
                    default:
                        QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ins->getKind()));
                }
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
    void exec(const comp::as::IntConstant &ins) {
        qvalue v;
        v.i = ins.getValue();
        f.setTemp(ins.getDest(), v);
    }

    void exec(const comp::as::GetLocal &ins) {
        f.setTemp(ins.getDest(), f.getLocal(ins.getLocalVariable()));
    }

    void exec(const comp::as::SetLocal &ins) {
        f.setLocal(ins.getLocalVariable(), f.getTemp(ins.getSrc()));
    }

    void exec(const comp::as::LoadString &ins) {
        qvalue v;
        v.p = ins.getString();
        f.setTemp(ins.getDest(), v);
    }

    void exec(const comp::as::RefInc &ins) {
        qvalue v = f.getTemp(ins.getTemp());
        if (v.p) {
            v.p->incRefCount();
        }
    }

    void exec(const comp::as::RefDec &ins) {
        qvalue v = f.getTemp(ins.getTemp());
        if (v.p) {
            v.p->decRefCount();
        }
    }

    void exec(const comp::as::ReadLockGlobal &ins) {
        ins.getGlobalVariable().readLock();
    }

    void exec(const comp::as::ReadUnlockGlobal &ins) {
        ins.getGlobalVariable().readUnlock();
    }

    void exec(const comp::as::WriteLockGlobal &ins) {
        ins.getGlobalVariable().writeLock();
    }

    void exec(const comp::as::WriteUnlockGlobal &ins) {
        ins.getGlobalVariable().writeUnlock();
    }

    void exec(const comp::as::GetGlobal &ins) {
        f.setTemp(ins.getDest(), ins.getGlobalVariable().getValue());
    }

    void exec(const comp::as::SetGlobal &ins) {
        ins.getGlobalVariable().setValue(f.getTemp(ins.getSrc()));
    }

    void exec(const comp::as::MakeGlobal &ins) {
        ins.getGlobalVariable().initValue(f.getTemp(ins.getInitValue()));
    }

    void exec(const comp::as::BinaryOperator &ins) {
        f.setTemp(ins.getDest(),
                ins.getOperator().getFunction()(f.getTemp(ins.getLeft()), f.getTemp(ins.getRight())));
    }

    void exec(const comp::as::Conversion &ins) {
        f.setTemp(ins.getDest(), ins.getConversion().getFunction()(f.getTemp(ins.getArg())));
    }

private:
    F &f;
};

} // namespace in
} // namespace qore

#endif // INCLUDE_QORE_IN_FUNCTIONINTERPRETER_H_
