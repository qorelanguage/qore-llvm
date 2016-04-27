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
/// \brief CompoundAssignmentExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_COMPOUNDASSIGNMENTEXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_COMPOUNDASSIGNMENTEXPRESSION_H_

#include "qore/comp/sem/expr/Expression.h"

namespace qore {
namespace comp {
namespace sem {

class CompoundAssignmentExpression : public Expression {

public:
    using Ptr = std::unique_ptr<CompoundAssignmentExpression>;

public:
    static Ptr create(Expression::Ptr left, const qore::BinaryOperator &op, Expression::Ptr right) {
        return Ptr(new CompoundAssignmentExpression(std::move(left), op, std::move(right)));
    }

    Kind getKind() const override {
        return Kind::CompoundAssignment;
    }

    const Type &getType() const override {
        return left->getType();
    }

    const Expression &getLeft() const {
        return *left;
    }

    const qore::BinaryOperator &getOperator() const {
        return op;
    }

    const Expression &getRight() const {
        return *right;
    }

private:
    CompoundAssignmentExpression(Expression::Ptr left, const qore::BinaryOperator &op, Expression::Ptr right)
            : left(std::move(left)), op(op), right(std::move(right)) {
    }

private:
    Expression::Ptr left;
    const qore::BinaryOperator &op;
    Expression::Ptr right;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_COMPOUNDASSIGNMENTEXPRESSION_H_
