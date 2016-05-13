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
/// \brief Defines the InvokeBinaryOperator instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_INVOKEBINARYOPERATOR_H_
#define INCLUDE_QORE_CORE_CODE_INVOKEBINARYOPERATOR_H_

#include "qore/core/BinaryOperator.h"
#include "qore/core/code/Instruction.h"
#include "qore/core/code/Temp.h"

namespace qore {
namespace code {

/**
 * \brief Instruction that executes a binary operator on two temporaries and stores the result in another temporary.
 */
class InvokeBinaryOperator : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param dest the destination temporary
     * \param op the binary operator
     * \param left the temporary holding the left operand
     * \param right the temporary holding the right operand
     * \param lpad optional landing pad in case an exception occurs
     */
    InvokeBinaryOperator(Temp dest, const BinaryOperator &op, Temp left, Temp right, const Block *lpad)
            : dest(dest), op(op), left(left), right(right), lpad(lpad) {
    }

    Kind getKind() const override {
        return Kind::InvokeBinaryOperator;
    }

    const Block *getLpad() const override {
        return lpad;
    }

    /**
     * \brief Returns the destination temporary.
     * @return the destination temporary
     */
    Temp getDest() const {
        return dest;
    }

    /**
     * \brief Returns the binary operator.
     * @return the binary operator
     */
    const BinaryOperator &getOperator() const {
        return op;
    }

    /**
     * \brief Returns the temporary holding the left operand.
     * @return the temporary holding the left operand
     */
    Temp getLeft() const {
        return left;
    }

    /**
     * \brief Returns the temporary holding the right operand.
     * @return the temporary holding the right operand
     */
    Temp getRight() const {
        return right;
    }

private:
    Temp dest;
    const BinaryOperator &op;
    Temp left;
    Temp right;
    const Block *lpad;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_INVOKEBINARYOPERATOR_H_
