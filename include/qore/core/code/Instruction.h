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
#ifndef INCLUDE_QORE_CORE_CODE_INSTRUCTION_H_
#define INCLUDE_QORE_CORE_CODE_INSTRUCTION_H_

#include <memory>

namespace qore {
namespace code {

/**
 * \brief Represents an instruction.
 */
class Instruction {

public:
    using Ptr = std::unique_ptr<Instruction>;       //!< Pointer type.

public:
    /**
     * \brief Identifies the kind of an instruction.
     */
    enum class Kind {
        BinaryOperator,     //!< Identifies an instance of \ref BinaryOperator.
        Conversion,         //!< Identifies an instance of \ref Conversion.
        Branch,             //!< Identifies an instance of \ref Branch.
        GetArg,             //!< Identifies an instance of \ref GetArg.
        GetGlobal,          //!< Identifies an instance of \ref GetGlobal.
        GetLocal,           //!< Identifies an instance of \ref GetLocal.
        IntConstant,        //!< Identifies an instance of \ref IntConstant.
        Jump,               //!< Identifies an instance of \ref Jump.
        LoadString,         //!< Identifies an instance of \ref LoadString.
        MakeGlobal,         //!< Identifies an instance of \ref MakeGlobal.
        ReadLockGlobal,     //!< Identifies an instance of \ref ReadLockGlobal.
        ReadUnlockGlobal,   //!< Identifies an instance of \ref ReadUnlockGlobal.
        RefDec,             //!< Identifies an instance of \ref RefDec.
        RefDecNoexcept,     //!< Identifies an instance of \ref RefDecNoexcept.
        RefInc,             //!< Identifies an instance of \ref RefInc.
        Ret,                //!< Identifies an instance of \ref Ret.
        RetVoid,            //!< Identifies an instance of \ref RetVoid.
        Rethrow,            //!< Identifies an instance of \ref Rethrow.
        SetGlobal,          //!< Identifies an instance of \ref SetGlobal.
        SetLocal,           //!< Identifies an instance of \ref SetLocal.
        WriteLockGlobal,    //!< Identifies an instance of \ref WriteLockGlobal.
        WriteUnlockGlobal,  //!< Identifies an instance of \ref WriteUnlockGlobal.
    };

public:
    virtual ~Instruction() = default;

    /**
     * \brief Returns the kind of the instruction represented by this instance.
     * \return the kind of the instruction represented by this instance
     */
    virtual Kind getKind() const = 0;

    /**
     * \brief Returns the basic block which should be executed in case an exception is thrown.
     * \return the landing pad block or nullptr if the exception does not need to be caught here
     */
    virtual const class Block *getLpad() const {
        return nullptr;
    }

    /**
     * \brief Returns true if this instruction serves as a block terminator.
     * \return true if this instruction serves as a block terminator.
     * \see Block
     */
    bool isTerminator() const {
        switch (getKind()) {
            case Kind::Rethrow:
            case Kind::Ret:
            case Kind::RetVoid:
            case Kind::Jump:
            case Kind::Branch:
                return true;
            default:
                return false;
        }
    }

protected:
    /**
     * \brief Default constructor.
     */
    Instruction() {
    }

private:
    Instruction(const Instruction &) = delete;
    Instruction(Instruction &&) = delete;
    Instruction &operator=(const Instruction &) = delete;
    Instruction &operator=(Instruction &&) = delete;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_INSTRUCTION_H_
