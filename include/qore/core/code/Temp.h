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
/// \brief Defines a type for temporary value storage.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_TEMP_H_
#define INCLUDE_QORE_CORE_CODE_TEMP_H_

#include "qore/core/Defs.h"

namespace qore {
namespace code {

/**
 * \brief Represents a temporary value storage during execution.
 *
 * Consider an expression `f() + a * b`. Before the addition can be performed, both subexpressions (`f()` and `a * b`)
 * need to be evaluated and their value stored somewhere. The analyzer produces code as follows (simplified):
 *  - evaluate `f()` and store the value info a temporary `t1`,
 *  - evaluate `a * b` and store the value info a temporary `t2`,
 *  - perform the addition of `t1` and `t2` and store the result into a temporary `t3`,
 *  - temporaries `t1` and `t2` are no longer needed and can be used in other subexpression calculations.
 * When a function is interpreted, the interpreter allocates an array of values (the length is calculated in advance
 * by the analyzer). A temporary is basically just an index into this array - however instead of using \ref Index
 * directly, a new type \ref Temp is introduced in order to prevent accidental mix-up with other entities such as
 * argument indices, local variables etc. Additionally, operations that do not make sense for this type
 * (e.g. default value, conversion to bool) are not included.
 * When a function is compiled into native code using LLVM, temporaries are translated into registers (with
 * possible spilling).
 */
class Temp {

public:
    /**
     * \brief Creates a new temporary with given index.
     * \param index the index of the temporary
     */
    explicit Temp(Index index) : index(index) {
    }

    /**
     * \brief Returns the index of the temporary.
     * \return the index of the temporary
     */
    Index getIndex() const {
        return index;
    }

    /**
     * \brief Default copy constructor.
     * \param src the instance to copy
     */
    Temp(const Temp &src) = default;

    /**
     * \brief Default move constructor.
     * \param src the instance to move
     */
    Temp(Temp &&src) = default;

    /**
     * \brief Default copy assignment.
     * \param src the instance to copy
     * \return this instance
     */
    Temp &operator=(const Temp &src) = default;

    /**
     * \brief Default move assignment.
     * \param src the instance to move
     * \return this instance
     */
    Temp &operator=(Temp &&src) = default;

    /**
     * \brief Compares this instance to another.
     * \param other the instance to compare this to
     * \return true if both instances represent the same temporary
     */
    bool operator==(const Temp &other) const {
        return index == other.index;
    }

    /**
     * \brief Compares this instance to another.
     * \param other the instance to compare this to
     * \return true if the instances represent different temporaries
     */
    bool operator!=(const Temp &other) const {
        return !(*this == other);
    }

private:
    Index index;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_TEMP_H_
