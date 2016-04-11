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
#include "qore/as/as.h"
#include "qore/rt/Func.h"

namespace qore {
namespace in {

class FunctionInterpreter {

public:
    FunctionInterpreter(as::F &f, std::vector<rt::qvalue> &strings, std::vector<rt::GlobalVariable *> &globals) : f(f),
            strings(strings), globals(globals), locals(f.localCount), temps(f.tempCount), b(f.blocks[0].get()), i(0) {
    }

    FunctionInterpreter(as::F &f, std::vector<rt::qvalue> &strings, std::vector<rt::GlobalVariable *> &globals,
            std::vector<rt::qvalue> &locals) : FunctionInterpreter(f, strings, globals) {
        std::copy(locals.begin(), locals.end(), this->locals.begin());
    }

    std::vector<rt::qvalue> getLocals() {
        return locals;
    }

    void run() {
        while (true) {
            assert(i < b->instructions.size());
            as::Instruction *ins = b->instructions[i++].get();

            switch (ins->getKind()) {
                case as::Instruction::Kind::IntConstant:
                    exec(static_cast<as::IntConstant *>(ins));
                    break;
                case as::Instruction::Kind::GetLocal:
                    exec(static_cast<as::GetLocal *>(ins));
                    break;
                case as::Instruction::Kind::SetLocal:
                    exec(static_cast<as::SetLocal *>(ins));
                    break;
                case as::Instruction::Kind::LoadString:
                    exec(static_cast<as::LoadString *>(ins));
                    break;
                case as::Instruction::Kind::RefInc:
                    exec(static_cast<as::RefInc *>(ins));
                    break;
                case as::Instruction::Kind::RefDec:
                    exec(static_cast<as::RefDec *>(ins));
                    break;
//                RefDecNoexcept,           6
                case as::Instruction::Kind::ReadLockGlobal:
                    exec(static_cast<as::ReadLockGlobal *>(ins));
                    break;
                case as::Instruction::Kind::ReadUnlockGlobal:
                    exec(static_cast<as::ReadUnlockGlobal *>(ins));
                    break;
                case as::Instruction::Kind::WriteLockGlobal:
                    exec(static_cast<as::WriteLockGlobal *>(ins));
                    break;
                case as::Instruction::Kind::WriteUnlockGlobal:
                    exec(static_cast<as::WriteUnlockGlobal *>(ins));
                    break;
                case as::Instruction::Kind::GetGlobal:
                    exec(static_cast<as::GetGlobal *>(ins));
                    break;
                case as::Instruction::Kind::SetGlobal:
                    exec(static_cast<as::SetGlobal *>(ins));
                    break;
                case as::Instruction::Kind::MakeGlobal:
                    exec(static_cast<as::MakeGlobal *>(ins));
                    break;
                case as::Instruction::Kind::FreeGlobal:
                    exec(static_cast<as::FreeGlobal *>(ins));
                    break;
//                LandingPad,               15
//                Rethrow,                  16
                case as::Instruction::Kind::BinaryOperator:
                    exec(static_cast<as::BinaryOperator *>(ins));
                    break;
                case as::Instruction::Kind::Conversion:
                    exec(static_cast<as::Conversion *>(ins));
                    break;
                case as::Instruction::Kind::RetVoid:
                    return;
                default:
                    QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ins->getKind()));
            }
        }
    }

private:
    void exec(as::IntConstant *ins) {
        temps[ins->getDest()] = rt::qint_to_qvalue(ins->getValue());
    }

    void exec(as::GetLocal *ins) {
        temps[ins->getDest()] = locals[ins->getSlot()];
    }

    void exec(as::SetLocal *ins) {
        locals[ins->getSlot()] = temps[ins->getSrc()];
    }

    void exec(as::LoadString *ins) {
        temps[ins->getDest()] = strings[ins->getId()];
    }

    void exec(as::RefInc *ins) {
        rt::incRef(temps[ins->getTemp()]);
    }

    void exec(as::RefDec *ins) {
        try {
            rt::decRef(temps[ins->getTemp()]);
        } catch (rt::Exception &e) {
            caught(e, ins->getLpad());
        }
    }

    void exec(as::ReadLockGlobal *ins) {
        rt::gv_read_lock(globals[ins->getId()]);
    }

    void exec(as::ReadUnlockGlobal *ins) {
        rt::gv_read_unlock(globals[ins->getId()]);
    }

    void exec(as::WriteLockGlobal *ins) {
        rt::gv_write_lock(globals[ins->getId()]);
    }

    void exec(as::WriteUnlockGlobal *ins) {
        rt::gv_write_unlock(globals[ins->getId()]);
    }

    void exec(as::GetGlobal *ins) {
        temps[ins->getDest()] = rt::gv_get(globals[ins->getId()]);
    }

    void exec(as::SetGlobal *ins) {
        rt::gv_set(globals[ins->getId()], temps[ins->getSrc()]);
    }

    void exec(as::MakeGlobal *ins) {
        globals[ins->getId()] = rt::gv_create(temps[ins->getSrc()]);    //TODO exception
    }

    void exec(as::FreeGlobal *ins) {
        temps[ins->getDest()] = rt::gv_free(globals[ins->getId()]);
    }

    void exec(as::BinaryOperator *ins) {
        try {
            temps[ins->getDest()] = ins->getDesc().f(temps[ins->getLeft()], temps[ins->getRight()]);
        } catch (rt::Exception &e) {
            caught(e, ins->getLpad());
        }
    }

    void exec(as::Conversion *ins) {
        try {
            temps[ins->getDest()] = ins->getDesc().f(temps[ins->getArg()]);
        } catch (rt::Exception &e) {
            caught(e, ins->getLpad());
        }
    }

private:
    void caught(rt::Exception &e, as::Id lpad) {
        assert(lpad != as::InvalidId);
        b = f.blocks[lpad].get();
        i = 1;
        //temps[static_cast<as::LandingPad *>(b->instructions[0].get())->getDest()] = e;
        QORE_NOT_IMPLEMENTED("");
    }

private:
    as::F &f;
    std::vector<rt::qvalue> &strings;
    std::vector<rt::GlobalVariable *> &globals;
    std::vector<rt::qvalue> locals;
    std::vector<rt::qvalue> temps;
    as::Block *b;
    as::Id i;
};

} // namespace in
} // namespace qore

#endif // INCLUDE_QORE_IN_FUNCTIONINTERPRETER_H_
