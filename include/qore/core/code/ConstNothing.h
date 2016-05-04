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
/// \brief Defines the ConstNothing instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_CONSTNOTHING_H_
#define INCLUDE_QORE_CORE_CODE_CONSTNOTHING_H_

#include "qore/core/code/Instruction.h"
#include "qore/core/code/Temp.h"

namespace qore {
namespace code {

/**
 * \brief Instruction that stores the NOTHING constant to a temporary.
 */
class ConstNothing : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param dest the temporary to load the constant into
     */
    explicit ConstNothing(Temp dest) : dest(dest) {
    }

    Kind getKind() const override {
        return Kind::ConstNothing;
    }

    /**
     * \brief Return the temporary to load the constant into.
     * \return the temporary to load the constant into
     */
    Temp getDest() const {
        return dest;
    }

private:
    Temp dest;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_CONSTNOTHING_H_
