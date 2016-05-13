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
/// \brief Defines the Branch instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_BRANCH_H_
#define INCLUDE_QORE_CORE_CODE_BRANCH_H_

#include "qore/core/code/Instruction.h"
#include "qore/core/code/Temp.h"

namespace qore {
namespace code {

/**
 * \brief A terminator instruction that jumps to one of two blocks depending on a condition.
 */
class Branch : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param condition the temporary holding the value of the condition (must be a boolean value)
     * \param trueDest the block to jump to in case the condition is true
     * \param falseDest the block to jump to in case the condition is false
     */
    Branch(Temp condition, const Block &trueDest, const Block &falseDest) : condition(condition), trueDest(trueDest),
            falseDest(falseDest) {
    }

    Kind getKind() const override {
        return Kind::Branch;
    }

    /**
     * \brief Returns the temporary holding the value of the condition.
     * \return the temporary holding the value of the condition
     */
    Temp getCondition() const {
        return condition;
    }

    /**
     * \brief Returns the block to jump to in case the condition is true.
     * \return the block to jump to in case the condition is true
     */
    const Block &getTrueDest() const {
        return trueDest;
    }

    /**
     * \brief Returns the block to jump to in case the condition is false.
     * \return the block to jump to in case the condition is false
     */
    const Block &getFalseDest() const {
        return falseDest;
    }

private:
    Temp condition;
    const Block &trueDest;
    const Block &falseDest;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_BRANCH_H_
