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
/// \brief Defines the InvokeFunction instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_INVOKEFUNCTION_H_
#define INCLUDE_QORE_CORE_CODE_INVOKEFUNCTION_H_

#include <vector>
#include "qore/core/code/Instruction.h"
#include "qore/core/code/Temp.h"

namespace qore {

class Function;

namespace code {

/**
 * \brief Instruction that executes a function and stores the return value in a temporary.
 */
class InvokeFunction : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param dest the destination temporary
     * \param function the function
     * \param args the temporaries holding the arguments
     * \param lpad optional landing pad in case an exception occurs
     */
    InvokeFunction(Temp dest, const Function &function, std::vector<Temp> args, const Block *lpad)
            : dest(dest), function(function), args(std::move(args)), lpad(lpad) {
    }

    Kind getKind() const override {
        return Kind::InvokeFunction;
    }

    const Block *getLpad() const override {
        return lpad;
    }

    /**
     * \brief Returns the destination temporary.
     * \return the destination temporary
     */
    Temp getDest() const {
        return dest;
    }

    /**
     * \brief Returns the function.
     * \return the function
     */
    const Function &getFunction() const {
        return function;
    }

    /**
     * \brief Returns the temporaries holding the arguments.
     * \return the temporaries holding the arguments
     */
    const std::vector<Temp> &getArgs() const {
        return args;
    }

private:
    Temp dest;
    const Function &function;
    std::vector<Temp> args;
    const Block *lpad;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_INVOKEFUNCTION_H_
