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
/// \brief InvokeBinaryOperatorExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_INVOKEBINARYOPERATOREXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_INVOKEBINARYOPERATOREXPRESSION_H_

#include "qore/comp/sem/expr/Expression.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Represents binary expressions.
 */
class InvokeBinaryOperatorExpression : public Expression {

public:
    using Ptr = std::unique_ptr<InvokeBinaryOperatorExpression>;        //!< Pointer type.

public:
    /**
     * \brief Creates a new instance.
     * \param op the binary operator
     * \param left the left subexpression
     * \param right the right subexpression
     * \return the new instance
     */
    static Ptr create(const BinaryOperator &op, Expression::Ptr left, Expression::Ptr right) {
        return Ptr(new InvokeBinaryOperatorExpression(op, std::move(left), std::move(right)));
    }

    Kind getKind() const override {
        return Kind::InvokeBinaryOperator;
    }

    const Type &getType() const override {
        return op.getResultType();
    }

    /**
     * \brief Returns the binary operator.
     * \return the binary operator
     */
    const BinaryOperator &getOperator() const {
        return op;
    }

    /**
     * \brief Returns the left subexpression.
     * \return the left subexpression
     */
    const Expression &getLeft() const {
        return *left;
    }

    /**
     * \brief Returns the right subexpression.
     * \return the right subexpression
     */
    const Expression &getRight() const {
        return *right;
    }

private:
    InvokeBinaryOperatorExpression(const BinaryOperator &op, Expression::Ptr left,
            Expression::Ptr right) : op(op), left(std::move(left)), right(std::move(right)) {
    }

private:
    const BinaryOperator &op;
    Expression::Ptr left;
    Expression::Ptr right;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_INVOKEBINARYOPERATOREXPRESSION_H_
