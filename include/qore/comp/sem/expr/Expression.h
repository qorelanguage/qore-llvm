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
class FunctionCallExpression;
class FunctionGroupExpression;
class GlobalVariableRefExpression;
class IntLiteralExpression;
class InvokeBinaryOperatorExpression;
class InvokeConversionExpression;
class LocalVariableInitExpression;
class LocalVariableRefExpression;
class NothingLiteralExpression;
class StringLiteralRefExpression;

/**
 * \brief Base class for temporary representations of expressions.
 */
class Expression {

public:
    /**
     * \brief Identifies the concrete type of an instance.
     */
    enum class Kind {
        Assignment,             //!< Identifies an instance of \ref AssignmentExpression.
        CompoundAssignment,     //!< Identifies an instance of \ref CompoundAssignmentExpression.
        FunctionCall,           //!< Identifies an instance of \ref FunctionCallExpression.
        FunctionGroup,          //!< Identifies an instance of \ref FunctionGroupExpression.
        GlobalVariableRef,      //!< Identifies an instance of \ref GlobalVariableRefExpression.
        IntLiteral,             //!< Identifies an instance of \ref IntLiteralExpression.
        InvokeBinaryOperator,   //!< Identifies an instance of \ref InvokeBinaryOperatorExpression.
        InvokeConversion,       //!< Identifies an instance of \ref InvokeConversionExpression.
        LocalVariableInit,      //!< Identifies an instance of \ref LocalVariableInitExpression.
        LocalVariableRef,       //!< Identifies an instance of \ref LocalVariableRefExpression.
        NothingLiteral,         //!< Identifies an instance of \ref NothingLiteralExpression.
        StringLiteralRef,       //!< Identifies an instance of \ref StringLiteralRefExpression.
    };

public:
    using Ptr = std::unique_ptr<Expression>;        //!< Pointer type.

public:
    virtual ~Expression() = default;

    /**
     * \brief Returns the kind of the expression.
     * \return the kind of the expression
     */
    virtual Kind getKind() const = 0;

    /**
     * \brief Returns the type of the expression.
     * \return the type of the expression
     */
    virtual const Type &getType() const = 0;

    /**
     * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Expression.
     * \param visitor the visitor to call
     * \return the value returned from the visitor
     * \tparam V the type of the visitor
     */
    template<typename V>
    typename V::ReturnType accept(V &visitor) const {
        /// \cond NoDoxygen
        #define CASE(K) case Kind::K: return visitor.visit(static_cast<const K ## Expression &>(*this));
        /// \endcond NoDoxygen
        switch (getKind()) {
            CASE(Assignment);
            CASE(CompoundAssignment);
            CASE(FunctionCall);
            CASE(FunctionGroup);
            CASE(GlobalVariableRef);
            CASE(IntLiteral);
            CASE(InvokeBinaryOperator);
            CASE(InvokeConversion);
            CASE(LocalVariableInit);
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
