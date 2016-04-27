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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CONVERSION_H_
#define INCLUDE_QORE_CONVERSION_H_

#include <string>
#include "qore/Type.h"
#include "qore/Value.h"

namespace qore {

class Conversion {

public:
    using Function = qvalue(qvalue);

public:
    static const Conversion *find(const Type &from, const Type &to);

    const std::string &getFunctionName() const {
        return functionName;
    }

    const Function &getFunction() const {
        return function;
    }

    const Type &getToType() const {
        return to;
    }

    bool canThrow() const {
        return throws;
    }

private:
    Conversion(std::string functionName, const Function &function, const Type &from, const Type &to, bool throws)
            : functionName(std::move(functionName)), function(function), from(from), to(to), throws(throws) {
    }

private:
    Conversion(const Conversion &) = delete;
    Conversion(Conversion &&) = delete;
    Conversion &operator=(const Conversion &) = delete;
    Conversion &operator=(Conversion &&) = delete;

private:
    static const Conversion AnyToString;
    static const Conversion IntToAny;
    static const Conversion IntToBool;
    static const Conversion IntToString;
    static const Conversion StringToInt;

private:
    std::string functionName;
    const Function &function;
    const Type &from;
    const Type &to;
    bool throws;
};

} // namespace qore

#endif // INCLUDE_QORE_CONVERSION_H_
