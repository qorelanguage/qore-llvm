//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 - 2020 Qore Technologies, s.r.o.
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
/// \brief FunctionGroupExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_FUNCTIONGROUPEXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_FUNCTIONGROUPEXPRESSION_H_

#include "qore/comp/sem/FunctionGroupInfo.h"
#include "qore/comp/sem/expr/Expression.h"

namespace qore {
namespace comp {
namespace sem {

class FunctionGroupInfo;

/**
 * \brief Represents a resolved function group.
 */
class FunctionGroupExpression : public Expression {

public:
    using Ptr = std::unique_ptr<FunctionGroupExpression>;           //!< Pointer type.

public:
    /**
     * \brief Creates a new instance.
     * \param functionGroup the function group represented by the expression
     * \return the new instance
     */
    static Ptr create(const FunctionGroupInfo &functionGroup) {
        return Ptr(new FunctionGroupExpression(functionGroup));
    }

    Kind getKind() const override {
        return Kind::FunctionGroup;
    }

    const Type &getType() const override {
        return Type::FunctionGroup;
    }

    /**
     * \brief Returns the function group represented by the expression.
     * \return the function group represented by the expression
     */
    const FunctionGroupInfo &getFunctionGroup() const {
        return functionGroup;
    }

private:
    explicit FunctionGroupExpression(const FunctionGroupInfo &functionGroup) : functionGroup(functionGroup) {
    }

private:
    const FunctionGroupInfo &functionGroup;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_FUNCTIONGROUPEXPRESSION_H_
