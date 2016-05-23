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
/// \brief Defines a class that describes binary operators.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_BINARYOPERATOR_H_
#define INCLUDE_QORE_CORE_BINARYOPERATOR_H_

#include <string>
#include "qore/core/Type.h"
#include "qore/core/Value.h"

namespace qore {

/**
 * \brief Describes binary operators.
 */
class BinaryOperator {

public:
    /**
     * \brief Enumeration of all binary operators.
     */
    enum class Kind {
        Plus,           //!< The '+' operator.
        PlusEquals,     //!< The '+=' operator.
    };

public:
    using Function = qvalue(qvalue, qvalue);    //!< The type of the function that implements the binary operator.

public:
    /**
     * \brief Finds a binary operator of given kind for given types of operands.
     * \param kind the kind of the binary operator
     * \param left the type of the left operand
     * \param right the type of the right operand
     * \return the binary operator
     * \throws Exception when no binary operator is applicable to given argument types
     */
    static const BinaryOperator &find(Kind kind, const Type &left, const Type &right);

    /**
     * \brief Returns the name of the function that implements the binary operator.
     * \return the name of the function that implements the binary operator
     */
    const std::string &getFunctionName() const {
        return functionName;
    }

    /**
     * \brief Returns a reference to the function that implements the binary operator.
     * \return a reference to the function that implements the binary operator
     */
    const Function &getFunction() const {
        return function;
    }

    /**
     * \brief Returns the type of the left operand.
     * \return the type of the left operand
     */
    const Type &getLeftType() const {
        return left;
    }

    /**
     * \brief Returns the type of the right operand.
     * \return the type of the right operand
     */
    const Type &getRightType() const {
        return right;
    }

    /**
     * \brief Returns the type of the result.
     * \return the type of the result
     */
    const Type &getResultType() const {
        return result;
    }

    /**
     * \brief Returns true if the binary operator can throw an exception at runtime.
     * \return true if the binary operator can throw an exception at runtime
     */
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

/**
 * \brief Writes the name of the kind of binary operator to an output stream.
 * \param os the output stream
 * \param kind the kind of the binary operator
 * \return the output stream
 */
std::ostream &operator<<(std::ostream &os, BinaryOperator::Kind kind);

} // namespace qore

#endif // INCLUDE_QORE_CORE_BINARYOPERATOR_H_
