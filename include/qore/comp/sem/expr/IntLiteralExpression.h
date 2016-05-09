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
/// \brief IntLiteralExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_INTLITERALEXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_INTLITERALEXPRESSION_H_

#include "qore/comp/sem/expr/Expression.h"
#include "qore/core/Value.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Represents an integer literal.
 */
class IntLiteralExpression : public Expression {

public:
    using Ptr = std::unique_ptr<IntLiteralExpression>;      //!< Pointer type.

public:
    /**
     * \brief Creates a new instance.
     * \param value the constant value
     * \return the new instance
     */
    static Ptr create(qint value) {
        return Ptr(new IntLiteralExpression(value));
    }

    Kind getKind() const override {
        return Kind::IntLiteral;
    }

    const Type &getType() const override {
        return Type::Int;
    }

    /**
     * \brief Returns the constant value.
     * \return the constant value
     */
    qint getValue() const {
        return value;
    }

private:
    explicit IntLiteralExpression(qint value) : value(value) {
    }

private:
    qint value;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_INTLITERALEXPRESSION_H_
