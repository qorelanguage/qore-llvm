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
/// \brief Function interpreter visitor.
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

/**
 * \brief A stack frame, a.k.a activation record. Holds the values of locals and temporaries.
 */
struct Frame {
    /**
     * \brief Constructor.
     * \param tempsCount the (initial) number of temporaries
     * \param localsCount the (initial) number of local variables
     */
    Frame(Size tempsCount, Size localsCount) : temps(tempsCount), locals(localsCount) {
        returnValue.p = nullptr;
    }

    std::vector<qvalue> temps;      //!< The values of temporaries.
    std::vector<qvalue> locals;     //!< The values of local variables.
    qvalue returnValue;
};

/**
 * \brief Implements the 'ref code::Instruction visitor that executes a function.
 */
class FunctionInterpreter {

public:
    /**
     * \brief Constructor.
     * \param frame the stack frame
     * \param entryBlock the entry block of the function
     */
    FunctionInterpreter(Frame &frame, code::Block &entryBlock)
            : frame(frame), done(false), it(entryBlock.begin()) {
    }

    /**
     * \brief Executes the interpreter.
     */
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

    ///\name Visitor implementation
    ///\{
    using ReturnType = void;

    void visit(const code::Branch &ins) {
        if (getTemp(ins.getCondition()).b) {
            it = ins.getTrueDest().begin();
        } else {
            it = ins.getFalseDest().begin();
        }
    }

    void visit(const code::ConstInt &ins) {
        qvalue v;
        v.i = ins.getValue();
        setTemp(ins.getDest(), v);
    }

    void visit(const code::ConstString &ins) {
        qvalue v;
        v.p = &ins.getString();
        setTemp(ins.getDest(), v);
    }

    void visit(const code::GlobalGet &ins) {
        setTemp(ins.getDest(), ins.getGlobalVariable().getValue());
    }

    void visit(const code::GlobalInit &ins) {
        ins.getGlobalVariable().initValue(getTemp(ins.getInitValue()));
    }

    void visit(const code::GlobalReadLock &ins) {
        ins.getGlobalVariable().readLock();
    }

    void visit(const code::GlobalReadUnlock &ins) {
        ins.getGlobalVariable().readUnlock();
    }

    void visit(const code::GlobalSet &ins) {
        ins.getGlobalVariable().setValue(getTemp(ins.getSrc()));
    }

    void visit(const code::GlobalWriteLock &ins) {
        ins.getGlobalVariable().writeLock();
    }

    void visit(const code::GlobalWriteUnlock &ins) {
        ins.getGlobalVariable().writeUnlock();
    }

    void visit(const code::InvokeBinaryOperator &ins) {
        setTemp(ins.getDest(), ins.getOperator().getFunction()(getTemp(ins.getLeft()), getTemp(ins.getRight())));
    }

    void visit(const code::InvokeConversion &ins) {
        setTemp(ins.getDest(), ins.getConversion().getFunction()(getTemp(ins.getArg())));
    }

    void visit(const code::InvokeFunction &ins) {
        const Function &f = ins.getFunction();
        Frame newFrame(f.getTempCount(), f.getLocalVariables().size());
        for (Index i = 0; i < ins.getArgs().size(); ++i) {
            newFrame.locals[i] = getTemp(ins.getArgs()[i]);
        }
        FunctionInterpreter fi(newFrame, f.getEntryBlock());
        fi.run();
        setTemp(ins.getDest(), newFrame.returnValue);
    }

    void visit(const code::Jump &ins) {
        it = ins.getDest().begin();
    }

    void visit(const code::LocalGet &ins) {
        setTemp(ins.getDest(), getLocal(ins.getLocalVariable()));
    }

    void visit(const code::LocalSet &ins) {
        setLocal(ins.getLocalVariable(), getTemp(ins.getSrc()));
    }

    void visit(const code::RefDec &ins) {
        qvalue v = getTemp(ins.getTemp());
        if (v.p) {
            v.p->decRefCount();
            //FIXME if an exception was created, throw it
        }
    }

    void visit(const code::RefInc &ins) {
        qvalue v = getTemp(ins.getTemp());
        if (v.p) {
            v.p->incRefCount();
        }
    }

    void visit(const code::Ret &ins) {
        frame.returnValue = getTemp(ins.getValue());
        done = true;
    }

    void visit(const code::RetVoid &ins) {
        done = true;
    }

    void visit(const code::Instruction &ins) {
        QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ins.getKind()));
    }
    ///\}

private:
    void setLocal(const LocalVariable &lv, qvalue value) {
        frame.locals[lv.getIndex()] = value;
    }

    qvalue getLocal(const LocalVariable &lv) {
        return frame.locals[lv.getIndex()];
    }

    void setTemp(code::Temp temp, qvalue value) {
        frame.temps[temp.getIndex()] = value;
    }

    qvalue getTemp(code::Temp temp) {
        return frame.temps[temp.getIndex()];
    }

private:
    Frame &frame;
    bool done;
    code::Block::Iterator it;
};

} // namespace in
} // namespace qore

#endif // INCLUDE_QORE_IN_FUNCTIONINTERPRETER_H_
