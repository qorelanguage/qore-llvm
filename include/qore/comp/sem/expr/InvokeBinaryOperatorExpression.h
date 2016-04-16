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

class InvokeBinaryOperatorExpression : public Expression {

public:
    using Ptr = std::unique_ptr<InvokeBinaryOperatorExpression>;

public:
    static Ptr create(const rt::meta::BinaryOperatorDesc &desc, Expression::Ptr left, Expression::Ptr right) {
        return Ptr(new InvokeBinaryOperatorExpression(desc, std::move(left), std::move(right)));
    }

    Kind getKind() const override {
        return Kind::InvokeBinaryOperator;
    }

    const as::Type &getType() const override {
        return as::Type::from(desc.retType);
    }

    const rt::meta::BinaryOperatorDesc &getDesc() const {
        return desc;
    }

    const Expression &getLeft() const {
        return *left;
    }

    const Expression &getRight() const {
        return *right;
    }

private:
    InvokeBinaryOperatorExpression(const rt::meta::BinaryOperatorDesc &desc, Expression::Ptr left,
            Expression::Ptr right) : desc(desc), left(std::move(left)), right(std::move(right)) {
    }

private:
    const rt::meta::BinaryOperatorDesc &desc;
    Expression::Ptr left;
    Expression::Ptr right;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_INVOKEBINARYOPERATOREXPRESSION_H_
