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
#define OUT(M)  return os << M << " @" << ins.location
    switch (ins.opcode) {
        case Opcode::Add:               OUT("ADD " << ins.location);
        case Opcode::Assign:            OUT("ASSIGN");
        case Opcode::Discard:           OUT("DISCARD");
        case Opcode::Dup:               OUT("DUP");
        case Opcode::LifetimeEnd:       OUT("LIFETIME_END " << *ins.variable);
        case Opcode::LifetimeStart:     OUT("LIFETIME_START " << *ins.variable);
        case Opcode::LoadUnique:        OUT("LOAD_UNIQUE");
        case Opcode::LoadVarPtr:        OUT("LOAD_VAR_PTR " << *ins.variable);
        case Opcode::Print:             OUT("PRINT");
        case Opcode::PushNothing:       OUT("PUSH_NOTHING");
        case Opcode::PushString:        OUT("PUSH_STRING " << *ins.stringLiteral);
        case Opcode::PushVar:           OUT("PUSH_VAR " << *ins.variable);
        case Opcode::Trim:              OUT("TRIM");
    }
#undef OUT
    QORE_UNREACHABLE("Invalid instruction Opcode: " << static_cast<int>(ins.opcode));
}

} // namespace qil
} // namespace qore
