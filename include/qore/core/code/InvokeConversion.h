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
/// \brief Defines the InvokeConversion instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_INVOKECONVERSION_H_
#define INCLUDE_QORE_CORE_CODE_INVOKECONVERSION_H_

#include "qore/core/Conversion.h"
#include "qore/core/code/Instruction.h"
#include "qore/core/code/Temp.h"

namespace qore {
namespace code {

/**
 * \brief Instruction that executes a conversion of a temporary value and stores the result in another temporary.
 */
class InvokeConversion : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param dest the destination temporary
     * \param conversion the conversion
     * \param arg the temporary holding the value to convert
     * \param lpad optional landing pad in case an exception occurs
     */
    InvokeConversion(Temp dest, const Conversion &conversion, Temp arg, const Block *lpad)
            : dest(dest), conversion(conversion), arg(arg), lpad(lpad) {
    }

    Kind getKind() const override {
        return Kind::InvokeConversion;
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
     * \brief Returns the conversion.
     * @return the conversion
     */
    const Conversion &getConversion() const {
        return conversion;
    }

    /**
     * \brief Returns the temporary holding the value to convert.
     * @return the temporary holding the value to convert
     */
    Temp getArg() const {
        return arg;
    }

private:
    Temp dest;
    const Conversion &conversion;
    Temp arg;
    const Block *lpad;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_INVOKECONVERSION_H_
