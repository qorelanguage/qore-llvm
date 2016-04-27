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
#include "qore/common/Exceptions.h"
#include "qore/comp/as/is.h"
#include "qore/rt/Context.h"

namespace qore {
namespace in {

template<typename F>
class FunctionInterpreter {

public:
    explicit FunctionInterpreter(rt::Context &ctx, F &f) : ctx(ctx), f(f) {
    }

    void run(comp::as::Block &bb) {
        const comp::as::Block *b = &bb;
        Id i = 0;
        while (true) {
            assert(i < b->instructions.size());
            comp::as::Instruction *ins = b->instructions[i++].get();
            try {
                switch (ins->getKind()) {
                    case comp::as::Instruction::Kind::IntConstant:
                        exec(static_cast<comp::as::IntConstant *>(ins));
                        break;
                    case comp::as::Instruction::Kind::GetLocal:
                        exec(static_cast<comp::as::GetLocal *>(ins));
                        break;
                    case comp::as::Instruction::Kind::SetLocal:
                        exec(static_cast<comp::as::SetLocal *>(ins));
                        break;
                    case comp::as::Instruction::Kind::LoadString:
                        exec(static_cast<comp::as::LoadString *>(ins));
                        break;
                    case comp::as::Instruction::Kind::RefInc:
                        exec(static_cast<comp::as::RefInc *>(ins));
                        break;
                    case comp::as::Instruction::Kind::RefDec:
                        exec(static_cast<comp::as::RefDec *>(ins));
                        break;
                    case comp::as::Instruction::Kind::ReadLockGlobal:
                        exec(static_cast<comp::as::ReadLockGlobal *>(ins));
                        break;
                    case comp::as::Instruction::Kind::ReadUnlockGlobal:
                        exec(static_cast<comp::as::ReadUnlockGlobal *>(ins));
                        break;
                    case comp::as::Instruction::Kind::WriteLockGlobal:
                        exec(static_cast<comp::as::WriteLockGlobal *>(ins));
                        break;
                    case comp::as::Instruction::Kind::WriteUnlockGlobal:
                        exec(static_cast<comp::as::WriteUnlockGlobal *>(ins));
                        break;
                    case comp::as::Instruction::Kind::GetGlobal:
                        exec(static_cast<comp::as::GetGlobal *>(ins));
                        break;
                    case comp::as::Instruction::Kind::SetGlobal:
                        exec(static_cast<comp::as::SetGlobal *>(ins));
                        break;
                    case comp::as::Instruction::Kind::MakeGlobal:
                        exec(static_cast<comp::as::MakeGlobal *>(ins));
                        break;
                    case comp::as::Instruction::Kind::BinaryOperator:
                        exec(static_cast<comp::as::BinaryOperator *>(ins));
                        break;
                    case comp::as::Instruction::Kind::Conversion:
                        exec(static_cast<comp::as::Conversion *>(ins));
                        break;
                    case comp::as::Instruction::Kind::RetVoid:
                        return;
                    case comp::as::Instruction::Kind::Jump: {
                        comp::as::Jump &ii = static_cast<comp::as::Jump &>(*ins);
                        b = &ii.getDest();
                        i = 0;
                        break;
                    }
                    default:
                        QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ins->getKind()));
                }
            } catch (Exception &e) {
                if (ins->getLpad()) {
                    QORE_UNREACHABLE("Exception thrown by an instruction with no landing pad");
                }
                //  set 'e' as the 'current exception'
                //      - it is only used during cleanup (by the refDecNoThrow instruction), i.e. the code between
                //        the start of the lpad block and either rethrow/resume or jump to user's catch block, which
                //        will store it to a user-defined local variable
                b = ins->getLpad();
                i = 0;
            }
        }
    }

private:
    void exec(comp::as::IntConstant *ins) {
        qvalue v;
        v.i = ins->getValue();
        f.setTemp(ins->getDest(), v);
    }

    void exec(comp::as::GetLocal *ins) {
        f.setTemp(ins->getDest(), f.getLocal(ins->getLocalVariable()));
    }

    void exec(comp::as::SetLocal *ins) {
        f.setLocal(ins->getLocalVariable(), f.getTemp(ins->getSrc()));
    }

    void exec(comp::as::LoadString *ins) {
        qvalue v;
        v.p = ins->getString();
        f.setTemp(ins->getDest(), v);
    }

    void exec(comp::as::RefInc *ins) {
        qvalue v = f.getTemp(ins->getTemp());
        if (v.p) {
            v.p->incRefCount();
        }
    }

    void exec(comp::as::RefDec *ins) {
        qvalue v = f.getTemp(ins->getTemp());
        if (v.p) {
            v.p->decRefCount();
        }
    }

    void exec(comp::as::ReadLockGlobal *ins) {
        ctx.gv_read_lock(ins->getGlobalVariable().getId());
    }

    void exec(comp::as::ReadUnlockGlobal *ins) {
        ctx.gv_read_unlock(ins->getGlobalVariable().getId());
    }

    void exec(comp::as::WriteLockGlobal *ins) {
        ctx.gv_write_lock(ins->getGlobalVariable().getId());
    }

    void exec(comp::as::WriteUnlockGlobal *ins) {
        ctx.gv_write_unlock(ins->getGlobalVariable().getId());
    }

    void exec(comp::as::GetGlobal *ins) {
        f.setTemp(ins->getDest(), ctx.gv_get(ins->getGlobalVariable().getId()));
    }

    void exec(comp::as::SetGlobal *ins) {
        ctx.gv_set(ins->getGlobalVariable().getId(), f.getTemp(ins->getSrc()));
    }

    void exec(comp::as::MakeGlobal *ins) {
        ctx.createGlobal(ins->getGlobalVariable().getId(), ins->getGlobalVariable().getType().isRefCounted(),
                f.getTemp(ins->getInitValue()));
        //TODO exception
    }

    void exec(comp::as::BinaryOperator *ins) {
        f.setTemp(ins->getDest(),
                ins->getOperator().getFunction()(f.getTemp(ins->getLeft()), f.getTemp(ins->getRight())));
    }

    void exec(comp::as::Conversion *ins) {
        f.setTemp(ins->getDest(), ins->getConversion().getFunction()(f.getTemp(ins->getArg())));
    }

private:
    rt::Context &ctx;
    F &f;
};

} // namespace in
} // namespace qore

#endif // INCLUDE_QORE_IN_FUNCTIONINTERPRETER_H_
