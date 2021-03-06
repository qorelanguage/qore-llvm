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
/// \brief InvokeConversionExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_INVOKECONVERSIONEXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_INVOKECONVERSIONEXPRESSION_H_

#include "qore/comp/sem/expr/Expression.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Represents an expression that converts the type of a value.
 */
class InvokeConversionExpression : public Expression {

public:
    using Ptr = std::unique_ptr<InvokeConversionExpression>;        //!< Pointer type.

public:
    /**
     * \brief Creates a new instance.
     * \param conversion the conversion to apply
     * \param arg the subexpression representing the value to convert
     * \return the new instance
     */
    static Ptr create(const qore::Conversion &conversion, Expression::Ptr arg) {
        return Ptr(new InvokeConversionExpression(conversion, std::move(arg)));
    }

    Kind getKind() const override {
        return Kind::InvokeConversion;
    }

    const Type &getType() const override {
        return conversion.getToType();
    }

    /**
     * \brief Returns the conversion to apply.
     * \return the conversion to apply
     */
    const qore::Conversion &getConversion() const {
        return conversion;
    }

    /**
     * \brief Returns the subexpression representing the value to convert.
     * \return the subexpression representing the value to convert
     */
    const Expression &getArg() const {
        return *arg;
    }

private:
    InvokeConversionExpression(const qore::Conversion &conversion, Expression::Ptr arg)
            : conversion(conversion), arg(std::move(arg)) {
    }

private:
    const qore::Conversion &conversion;
    Expression::Ptr arg;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_INVOKECONVERSIONEXPRESSION_H_
