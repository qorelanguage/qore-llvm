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
/// \brief LifetimeStartExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IR_EXPR_LIFETIMESTARTEXPRESSION_H_
#define INCLUDE_QORE_IR_EXPR_LIFETIMESTARTEXPRESSION_H_

#include "qore/ir/decl/LocalVariable.h"
#include "qore/ir/expr/Expression.h"

namespace qore {
namespace ir {

class LifetimeStartExpression : public Expression {

public:
    using Ptr = std::unique_ptr<LifetimeStartExpression>;

public:
    static Ptr create(const LocalVariable &localVariable, Expression::Ptr initExpression) {
        return Ptr(new LifetimeStartExpression(localVariable, std::move(initExpression)));
    }

    Kind getKind() const override {
        return Kind::LifetimeStart;
    }

    const Type &getType() const override {
        return localVariable.getType();
    }

    const LocalVariable &getLocalVariable() const {
        return localVariable;
    }

    const Expression &getInitExpression() const {
        return *initExpression;
    }

private:
    explicit LifetimeStartExpression(const LocalVariable &localVariable, Expression::Ptr initExpression)
            : localVariable(localVariable), initExpression(std::move(initExpression)) {
    }

private:
    const LocalVariable &localVariable;
    Expression::Ptr initExpression;
};

} // namespace ir
} // namespace qore

#endif // INCLUDE_QORE_IR_EXPR_LIFETIMESTARTEXPRESSION_H_
