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
/// \brief Defines the RetVoid instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_RETVOID_H_
#define INCLUDE_QORE_CORE_CODE_RETVOID_H_

#include "qore/core/code/Instruction.h"

namespace qore {
namespace code {

/**
 * \brief A terminator instruction that terminates the execution of current function returning control to the caller
 * with no return value.
 */
class RetVoid : public Instruction {

public:
    /**
     * \brief Constructor.
     */
    RetVoid() {
    }

    Kind getKind() const override {
        return Kind::RetVoid;
    }
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_RETVOID_H_
