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
#ifndef INCLUDE_QORE_COMP_AS_INSTRUCTION_H_
#define INCLUDE_QORE_COMP_AS_INSTRUCTION_H_

#include <memory>

namespace qore {
namespace comp {
namespace as {

class Block;
class Instruction {

public:
    using Ptr = std::unique_ptr<Instruction>;

public:
    enum Kind {
        IntConstant,
        GetLocal,
        SetLocal,
        LoadString,
        RefInc,
        RefDec,
        RefDecNoexcept,
        ReadLockGlobal,
        ReadUnlockGlobal,
        WriteLockGlobal,
        WriteUnlockGlobal,
        GetGlobal,
        SetGlobal,
        MakeGlobal,
        LandingPad,
        Rethrow,
        BinaryOperator,
        Conversion,
        Ret,
        RetVoid,
        MakeStringLiteral,
        GetArg,
    };

public:
    virtual ~Instruction() = default;

    virtual Kind getKind() const = 0;

    Block *getLpad() const {
        return lpad;
    }

protected:
    explicit Instruction(Block *lpad = nullptr) : lpad(lpad) {
    }

private:
    Instruction(const Instruction &) = delete;
    Instruction(Instruction &&) = delete;
    Instruction &operator=(const Instruction &) = delete;
    Instruction &operator=(Instruction &&) = delete;

private:
    Block *lpad;
};

} // namespace as
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AS_INSTRUCTION_H_
