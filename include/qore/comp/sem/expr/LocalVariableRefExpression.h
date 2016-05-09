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
/// \brief LocalVariableRefExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_LOCALVARIABLEREFEXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_LOCALVARIABLEREFEXPRESSION_H_

#include "qore/comp/sem/LocalVariableInfo.h"
#include "qore/comp/sem/expr/Expression.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Represents an expression that references a local variable.
 */
class LocalVariableRefExpression : public Expression {

public:
    using Ptr = std::unique_ptr<LocalVariableRefExpression>;        //!< Pointer type.

public:
    /**
     * \brief Creates a new instance.
     * \param localVariable the local variable referenced by the expression
     * \return the new instance
     */
    static Ptr create(const LocalVariableInfo &localVariable) {
        return Ptr(new LocalVariableRefExpression(localVariable));
    }

    Kind getKind() const override {
        return Kind::LocalVariableRef;
    }

    const Type &getType() const override {
        return localVariable.getType();
    }

    /**
     * \brief Returns the local variable referenced by the expression.
     * \return the local variable referenced by the expression
     */
    const LocalVariableInfo &getLocalVariable() const {
        return localVariable;
    }

private:
    explicit LocalVariableRefExpression(const LocalVariableInfo &localVariable) : localVariable(localVariable) {
    }

private:
    const LocalVariableInfo &localVariable;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_LOCALVARIABLEREFEXPRESSION_H_
