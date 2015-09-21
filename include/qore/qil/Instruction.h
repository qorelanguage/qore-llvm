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
#ifndef INCLUDE_QORE_QIL_INSTRUCTION_H_
#define INCLUDE_QORE_QIL_INSTRUCTION_H_

#include <ostream>
#include "qore/qil/Opcode.h"
#include "qore/qil/StringLiteral.h"
#include "qore/qil/Variable.h"

namespace qore {
namespace qil {

/**
 * \brief The structure of a QIL instruction.
 */
struct Instruction {
    Opcode opcode;                          //!< The opcode of the instruction.
    SourceLocation location;                //!< The source location associated with the instruction.
    union {
        Variable *variable;                 //!< A Variable argument of the instruction.
        StringLiteral *stringLiteral;       //!< A StringLiteral argument of the instruction.
    };
};

/**
 * \brief Writes an instruction to an output stream.
 * \param os the output stream
 * \param instruction the instruction
 * \return the output stream
 */
std::ostream &operator<<(std::ostream &os, const Instruction &instruction);

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_INSTRUCTION_H_
