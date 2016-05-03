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
/// \brief Defines the GlobalSet instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_GLOBALSET_H_
#define INCLUDE_QORE_CORE_CODE_GLOBALSET_H_

#include "qore/core/GlobalVariable.h"
#include "qore/core/code/Instruction.h"
#include "qore/core/code/Temp.h"

namespace qore {
namespace code {

/**
 * \brief Instruction that stores a temporary value to a global variable.
 *
 * It is assumed that the thread executing this instruction has acquired the appropriate lock.
 */
class GlobalSet : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param globalVariable the global variable
     * \param src the temporary to store into the global variable
     */
    GlobalSet(GlobalVariable &globalVariable, Temp src) : globalVariable(globalVariable), src(src) {
    }

    Kind getKind() const override {
        return Kind::GlobalSet;
    }

    /**
     * \brief Returns the global variable.
     * \return the global variable
     */
    GlobalVariable &getGlobalVariable() const {
        return globalVariable;
    }

    /**
     * \brief Returns the temporary to store into the global variable.
     * \return the temporary to store into the global variable
     */
    Temp getSrc() const {
        return src;
    }

private:
    GlobalVariable &globalVariable;
    Temp src;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_GLOBALSET_H_
