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
/// \brief A builder for QIL code.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_QIL_CODEBUILDER_H_
#define INCLUDE_QORE_QIL_CODEBUILDER_H_

#include <vector>
#include "qore/qil/Instruction.h"

namespace qore {
namespace qil {

/**
 * \brief Helper class for building Code instances.
 */
class CodeBuilder {

public:
    CodeBuilder() = default;

    /**
     * \brief Builds the code and resets the builder to default state.
     * \return built code
     */
    Code build() {
        return Code(std::move(instructions));
    }

    /// \name Builder methods
    /// \see Opcode
    /// \{
    CodeBuilder &add(const SourceLocation &location) {
        return ins(Opcode::Add, location);
    }

    CodeBuilder &assign(const SourceLocation &location) {
        return ins(Opcode::Assign, location);
    }

    CodeBuilder &discard(const SourceLocation &location) {
        return ins(Opcode::Discard, location);
    }

    CodeBuilder &dupCond(const SourceLocation &location, bool cond) {
        return cond ? ins(Opcode::Dup, location) : *this;
    }

    CodeBuilder &lifetimeEnd(const SourceLocation &location, Variable *var) {
        return ins(Opcode::LifetimeEnd, location, var);
    }

    CodeBuilder &lifetimeStart(const SourceLocation &location, Variable *var) {
        return ins(Opcode::LifetimeStart, location, var);
    }

    CodeBuilder &loadUnique(const SourceLocation &location) {
        return ins(Opcode::LoadUnique, location);
    }

    CodeBuilder &loadVarPtr(const SourceLocation &location, Variable *var) {
        return ins(Opcode::LoadVarPtr, location, var);
    }

    CodeBuilder &print(const SourceLocation &location) {
        return ins(Opcode::Print, location);
    }

    CodeBuilder &pushNothing(const SourceLocation &location) {
        return ins(Opcode::PushNothing, location);
    }

    CodeBuilder &pushString(const SourceLocation &location, StringLiteral *str) {
        return ins(Opcode::PushString, location, str);
    }

    CodeBuilder &pushVar(const SourceLocation &location, Variable *var) {
        return ins(Opcode::PushVar, location, var);
    }

    CodeBuilder &trim(const SourceLocation &location) {
        return ins(Opcode::Trim, location);
    }
    /// \}

private:
    Instruction &makeIns(Opcode opcode, const SourceLocation &location) {
        instructions.resize(instructions.size() + 1);
        Instruction &i = instructions.back();
        i.opcode = opcode;
        i.location = location;
        return i;
    }

    CodeBuilder &ins(Opcode opcode, const SourceLocation &location) {
        makeIns(opcode, location).variable = nullptr;
        return *this;
    }

    CodeBuilder &ins(Opcode opcode, const SourceLocation &location, Variable *var) {
        makeIns(opcode, location).variable = var;
        return *this;
    }

    CodeBuilder &ins(Opcode opcode, const SourceLocation &location, StringLiteral *str) {
        makeIns(opcode, location).stringLiteral = str;
        return *this;
    }

    std::vector<Instruction> instructions;
};

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_CODEBUILDER_H_
