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
/// \brief Defines the GlobalReadUnlock instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_GLOBALREADUNLOCK_H_
#define INCLUDE_QORE_CORE_CODE_GLOBALREADUNLOCK_H_

#include "qore/core/GlobalVariable.h"
#include "qore/core/code/Instruction.h"

namespace qore {
namespace code {

/**
 * \brief Instruction that releases the read lock for a global variable.
 *
 * It is assumed that the thread executing this instruction is holding the read lock.
 */
class GlobalReadUnlock : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param globalVariable the global variable
     */
    explicit GlobalReadUnlock(GlobalVariable &globalVariable) : globalVariable(globalVariable) {
    }

    Kind getKind() const override {
        return Kind::GlobalReadUnlock;
    }

    /**
     * \brief Returns the global variable.
     * \return the global variable
     */
    GlobalVariable &getGlobalVariable() const {
        return globalVariable;
    }

private:
    GlobalVariable &globalVariable;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_GLOBALREADUNLOCK_H_
