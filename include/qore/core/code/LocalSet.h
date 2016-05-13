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
/// \brief Defines the LocalSet instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_LOCALSET_H_
#define INCLUDE_QORE_CORE_CODE_LOCALSET_H_

#include "qore/core/LocalVariable.h"
#include "qore/core/code/Instruction.h"
#include "qore/core/code/Temp.h"

namespace qore {
namespace code {

/**
 * \brief Instruction that stores a temporary value to a local variable.
 */
class LocalSet : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param localVariable the local variable
     * \param src the temporary to store into the local variable
     */
    LocalSet(const LocalVariable &localVariable, Temp src) : localVariable(localVariable), src(src) {
    }

    Kind getKind() const override {
        return Kind::LocalSet;
    }

    /**
     * \brief Returns the local variable.
     * \return the local variable
     */
    const LocalVariable &getLocalVariable() const {
        return localVariable;
    }

    /**
     * \brief Returns the temporary to store into the local variable.
     * \return the temporary to store into the local variable
     */
    Temp getSrc() const {
        return src;
    }

private:
    const LocalVariable &localVariable;
    Temp src;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_LOCALSET_H_
