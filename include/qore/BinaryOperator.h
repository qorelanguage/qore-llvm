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
#ifndef INCLUDE_QORE_BINARYOPERATOR_H_
#define INCLUDE_QORE_BINARYOPERATOR_H_

#include <string>
#include "qore/Type.h"
#include "qore/Value.h"

namespace qore {

class BinaryOperator {

public:
    enum class Kind {
        Plus,
        PlusEquals,
    };

public:
    using Function = qvalue(qvalue, qvalue);

public:
    static const BinaryOperator &find(Kind kind, const Type &left, const Type &right);

    const std::string &getFunctionName() const {
        return functionName;
    }

    const Function &getFunction() const {
        return function;
    }

    const Type &getLeftType() const {
        return left;
    }

    const Type &getRightType() const {
        return right;
    }

    const Type &getResultType() const {
        return result;
    }

    bool canThrow() const {
        return throws;
    }

private:
    BinaryOperator(std::string functionName, const Function &function, Kind kind, const Type &left, const Type &right,
            const Type &result, bool throws) : functionName(std::move(functionName)), function(function),
            kind(kind), left(left), right(right), result(result), throws(throws) {
    }

private:
    BinaryOperator(const BinaryOperator &) = delete;
    BinaryOperator(BinaryOperator &&) = delete;
    BinaryOperator &operator=(const BinaryOperator &) = delete;
    BinaryOperator &operator=(BinaryOperator &&) = delete;

private:
    static const BinaryOperator AnyPlusAny;
    static const BinaryOperator AnyPlusEqualsAny;
    static const BinaryOperator SoftStringPlusSoftString;
    static const BinaryOperator SoftIntPlusSoftInt;

private:
    std::string functionName;
    const Function &function;
    Kind kind;
    const Type &left;
    const Type &right;
    const Type &result;
    bool throws;
};

std::ostream &operator<<(std::ostream &os, BinaryOperator::Kind kind);

} // namespace qore

#endif // INCLUDE_QORE_BINARYOPERATOR_H_
