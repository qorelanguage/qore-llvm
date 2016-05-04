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
/// \brief Expression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_EXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_EXPRESSION_H_

#include <memory>
#include "qore/core/Type.h"

namespace qore {
namespace comp {
namespace sem {

class AssignmentExpression;
class CompoundAssignmentExpression;
class GlobalVariableRefExpression;
class IntLiteralExpression;
class InvokeBinaryOperatorExpression;
class InvokeConversionExpression;
class LifetimeStartExpression;
class LocalVariableRefExpression;
class NothingLiteralExpression;
class StringLiteralRefExpression;

class Expression {

public:
    enum class Kind {
        Assignment,
        CompoundAssignment,
        GlobalVariableRef,
        IntLiteral,
        InvokeBinaryOperator,
        InvokeConversion,
        LifetimeStart,
        LocalVariableRef,
        NothingLiteral,
        StringLiteralRef,
    };

public:
    using Ptr = std::unique_ptr<Expression>;

public:
    virtual ~Expression() = default;
    virtual Kind getKind() const = 0;
    virtual const Type &getType() const = 0;

    template<typename V>
    typename V::ReturnType accept(V &v) const {
        #define CASE(K) case Kind::K: return v.visit(static_cast<const K ## Expression &>(*this));
        switch (getKind()) {
            CASE(Assignment);
            CASE(CompoundAssignment);
            CASE(GlobalVariableRef);
            CASE(IntLiteral);
            CASE(InvokeBinaryOperator);
            CASE(InvokeConversion);
            CASE(LifetimeStart);
            CASE(LocalVariableRef);
            CASE(NothingLiteral);
            CASE(StringLiteralRef);
            default:
                QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(getKind()));
        }
        #undef CASE
    }

protected:
    Expression() = default;

private:
    Expression(const Expression &) = delete;
    Expression(Expression &&) = delete;
    Expression &operator=(const Expression &) = delete;
    Expression &operator=(Expression &&) = delete;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_EXPRESSION_H_
