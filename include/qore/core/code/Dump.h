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
/// \brief Utility class for dumping code.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_DUMP_H_
#define INCLUDE_QORE_CORE_CODE_DUMP_H_

#include <map>
#include <queue>
#include <string>
#include "qore/core/Env.h"

namespace qore {
namespace code {

/// \cond IGNORED_BY_DOXYGEN
template<typename OS>
class Dump {

public:
    using ReturnType = void;

public:
    explicit Dump(OS &os, util::Indent indent) : os(os), indent(indent) {
    }

    void dump(const Block &b) {
        blockMap[&b] = 1;
        queue.push(&b);
        while (!queue.empty()) {
            const Block *b = queue.front();
            queue.pop();
            os << indent++ << "Block " << block(*b) << ":\n";
            dumpCode(*b);
            --indent;
        }
    }

    void dumpCode(const Block &b) {
        for (const Instruction &ins : b) {
            os << indent;
            ins.accept(*this);
            if (ins.getLpad()) {
                os << " with landing pad:\n";
                ++indent;
                dumpCode(*ins.getLpad());
                --indent;
            } else {
                os << "\n";
            }
        }
    }

    void visit(const Branch &ins) {
        os << "Branch " << temp(ins.getCondition()) << ", "
                << block(ins.getTrueDest()) << ", " << block(ins.getFalseDest());
    }

    void visit(const ConstInt &ins) {
        os << "ConstInt " << temp(ins.getDest()) << ", " << ins.getValue();
    }

    void visit(const ConstString &ins) {
        os << "ConstString " << temp(ins.getDest()) << ", \"";
        const std::string &s = ins.getString()->get();
        for (Index i = 0; i < s.length(); i++) {
            if (i > 15) {
                os << "...";
                break;
            }
            if (s[i] == '\n') {
                os << '\n';
            } else if (s[i] == '\\') {
                os << '\\';
            } else if (s[i] < 32 || s[i] > 127) {
                os << ' ';
            } else {
                os << s[i];
            }
        }
        os << "\"";
    }

    void visit(const GlobalGet &ins) {
        os << "GlobalGet " << temp(ins.getDest()) << ", " << global(ins.getGlobalVariable());
    }

    void visit(const GlobalInit &ins) {
        os << "GlobalInit " << global(ins.getGlobalVariable()) << ", " << temp(ins.getInitValue());
    }

    void visit(const GlobalReadLock &ins) {
        os << "GlobalReadLock " << global(ins.getGlobalVariable());
    }

    void visit(const GlobalReadUnlock &ins) {
        os << "GlobalReadUnlock " << global(ins.getGlobalVariable());
    }

    void visit(const GlobalSet &ins) {
        os << "GlobalSet " << global(ins.getGlobalVariable()) << ", " << temp(ins.getSrc());
    }

    void visit(const GlobalWriteLock &ins) {
        os << "GlobalWriteLock " << global(ins.getGlobalVariable());
    }

    void visit(const GlobalWriteUnlock &ins) {
        os << "GlobalWriteUnlock " << global(ins.getGlobalVariable());
    }

    void visit(const InvokeBinaryOperator &ins) {
        os << "InvokeBinaryOperator " << ins.getOperator().getFunctionName() << " " << temp(ins.getDest())
                << ", " << temp(ins.getLeft()) << ", " << temp(ins.getRight());
    }

    void visit(const InvokeConversion &ins) {
        os << "InvokeConversion " << ins.getConversion().getFunctionName() << " " << temp(ins.getDest())
                << ", " << temp(ins.getArg());
    }

    void visit(const Jump &ins) {
        os << "Jump " << block(ins.getDest());
    }

    void visit(const LocalGet &ins) {
        os << "LocalGet " << temp(ins.getDest()) << ", " << local(ins.getLocalVariable());
    }

    void visit(const LocalSet &ins) {
        os << "LocalSet " << local(ins.getLocalVariable()) << ", " << temp(ins.getSrc());
    }

    void visit(const RefDec &ins) {
        os << "RefDec " << temp(ins.getTemp());
    }

    void visit(const RefDecNoexcept &ins) {
        os << "RefDecNoexcept " << temp(ins.getTemp());
    }

    void visit(const RefInc &ins) {
        os << "RefInc " << temp(ins.getTemp());
    }

    void visit(const ResumeUnwind &ins) {
        os << "ResumeUnwind";
    }

    void visit(const Ret &ins) {
        os << "Ret " << temp(ins.getValue());
    }

    void visit(const RetVoid &ins) {
        os << "RetVoid";
    }

    void visit(const Instruction &ins) {
        QORE_NOT_IMPLEMENTED("Instruction::Kind " << static_cast<int>(ins.getKind()));
    }

private:
    std::string block(const Block &b) {
        Index &i = blockMap[&b];
        if (!i) {
            i = blockMap.size();
            queue.push(&b);
        }

        std::ostringstream str;
        str << " #" << i;
        return str.str();
    }

    std::string global(const GlobalVariable &gv) {
        std::ostringstream str;
        str << "our " << gv.getType() << " " << gv.getName();
        return str.str();
    }

    std::string local(const LocalVariable &lv) {
        std::ostringstream str;
        str << "local." << lv.getIndex();
        return str.str();
    }

    std::string temp(const Temp &t) {
        std::ostringstream str;
        str << "temp." << t.getIndex();
        return str.str();
    }

private:
    OS &os;
    util::Indent indent;
    std::map<const Block *, Index> blockMap;
    std::queue<const Block *> queue;
};

template<typename OS>
void dump(OS &os, Env &env) {
    Dump<OS> dump(os);
    dump.dump(env.getRootNamespace(), true);
}
/// \endcond IGNORED_BY_DOXYGEN

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_DUMP_H_
