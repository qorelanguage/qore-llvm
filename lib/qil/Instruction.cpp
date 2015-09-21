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
/// \brief QIL instruction.
///
//------------------------------------------------------------------------------
#include "qore/qil/Instruction.h"
#include "qore/common/Util.h"

namespace qore {
namespace qil {

std::ostream &operator<<(std::ostream &os, const Instruction &ins) {
    switch (ins.opcode) {
        case Opcode::LoadLocVarPtr:
            return os << "LOAD_PTR of " << *ins.variable;
        case Opcode::PushLocVar:
            return os << "PUSH " << *ins.variable;
        case Opcode::PushString:
            return os << "PUSH " << *ins.stringLiteral;
        case Opcode::Swap:
            return os << "SWAP";
        case Opcode::CleanupLValue:
            return os << "CLEANUP_LVALUE";
        case Opcode::PopAndDeref:
            return os << "POP_AND_DEREF";
        case Opcode::LoadUnique:
            return os << "LOAD_UNIQUE";
        case Opcode::Trim:
            return os << "TRIM";
        case Opcode::Dup:
            return os << "DUP";
        case Opcode::Add:
            return os << "ADD";
        case Opcode::PushNothing:
            return os << "PUSH_NOTHING";
        case Opcode::Print:
            return os << "PRINT";
        case Opcode::LifetimeStart:
            return os << "LIFETIME_START " << *ins.variable;
        case Opcode::LifetimeEnd:
            return os << "LIFETIME_END " << *ins.variable;
        case Opcode::Ret:
            return os << "RET";
    }
    QORE_UNREACHABLE("Invalid instruction Opcode: " << static_cast<int>(ins.opcode));
}

} // namespace qil
} // namespace qore
