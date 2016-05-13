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
/// \brief LocalVariableInitExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_LOCALVARIABLEINITEXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_LOCALVARIABLEINITEXPRESSION_H_

#include "qore/comp/sem/LocalVariableInfo.h"
#include "qore/comp/sem/expr/Expression.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Represents the initialization of a local variable.
 */
class LocalVariableInitExpression : public Expression {

public:
    using Ptr = std::unique_ptr<LocalVariableInitExpression>;       //!< Pointer type.

public:
    /**
     * \brief Creates a new instance.
     * \param localVariable the local variable
     * \param initExpression the subexpression representing the initialization value for the local variable
     * \return the new instance
     */
    static Ptr create(LocalVariableInfo &localVariable, Expression::Ptr initExpression) {
        return Ptr(new LocalVariableInitExpression(localVariable, std::move(initExpression)));
    }

    Kind getKind() const override {
        return Kind::LocalVariableInit;
    }

    const Type &getType() const override {
        return localVariable.getType();
    }

    /**
     * \brief Returns the local variable.
     * \return the local variable
     */
    LocalVariableInfo &getLocalVariable() const {
        return localVariable;
    }

    /**
     * \brief Returns the subexpression representing the initialization value for the local variable.
     * \return the subexpression representing the initialization value for the local variable
     */
    const Expression &getInitExpression() const {
        return *initExpression;
    }

private:
    LocalVariableInitExpression(LocalVariableInfo &localVariable, Expression::Ptr initExpression)
            : localVariable(localVariable), initExpression(std::move(initExpression)) {
    }

private:
    LocalVariableInfo &localVariable;
    Expression::Ptr initExpression;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_LOCALVARIABLEINITEXPRESSION_H_
