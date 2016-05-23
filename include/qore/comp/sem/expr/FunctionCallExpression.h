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
/// \brief FunctionCallExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_FUNCTIONCALLEXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_FUNCTIONCALLEXPRESSION_H_

#include <vector>
#include "qore/core/Function.h"
#include "qore/core/util/Iterators.h"
#include "qore/comp/sem/expr/Expression.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Represents a function call.
 */
class FunctionCallExpression : public Expression {

public:
    using Ptr = std::unique_ptr<FunctionCallExpression>;                //!< Pointer type.
    using Iterator = util::VectorOfPtrIteratorAdapter<Expression>;      //!< Argument iterator.

public:
    /**
     * \brief Creates a new instance.
     * \param function the function overload to be called
     * \param args the arguments
     * \return the new instance
     */
    static Ptr create(const Function &function, std::vector<Expression::Ptr> args) {
        return Ptr(new FunctionCallExpression(function, std::move(args)));
    }

    Kind getKind() const override {
        return Kind::FunctionCall;
    }

    const Type &getType() const override {
        return function.getType().getReturnType();
    }

    /**
     * \brief Returns the function overload to be called.
     * \return the function overload to be called
     */
    const Function &getFunction() const {
        return function;
    }

    /**
     * \brief Returns a range for iterating the arguments.
     * \return a range for iterating the arguments
     */
    util::IteratorRange<Iterator> getArgs() const {
        return util::IteratorRange<Iterator>(args);
    }

private:
    FunctionCallExpression(const Function &function, std::vector<Expression::Ptr> args)
            : function(function), args(std::move(args)) {
    }

private:
    const Function &function;
    std::vector<Expression::Ptr> args;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_FUNCTIONCALLEXPRESSION_H_
