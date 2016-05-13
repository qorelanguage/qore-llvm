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
/// \brief Defines the RefDecNoexcept instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_REFDECNOEXCEPT_H_
#define INCLUDE_QORE_CORE_CODE_REFDECNOEXCEPT_H_

#include "qore/core/code/Instruction.h"
#include "qore/core/code/Temp.h"

namespace qore {
namespace code {

/**
 * \brief Instruction that decreases the reference count of a temporary value as part of a cleanup during
 * stack unwinding.
 *
 * Unlike \ref RefDec, this instruction must not throw an exception since there is an exception already being
 * handled. Instead, any exceptions from destructors need to be linked to the current exception.
 */
class RefDecNoexcept : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param temp the temporary value whose reference count should be decreased
     */
    explicit RefDecNoexcept(Temp temp) : temp(temp) {
    }

    Kind getKind() const override {
        return Kind::RefDecNoexcept;
    }

    /**
     * \brief Returns the temporary value whose reference count should be decreased.
     * \return the temporary value whose reference count should be decreased
     */
    Temp getTemp() const {
        return temp;
    }

private:
    Temp temp;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_REFDECNOEXCEPT_H_
