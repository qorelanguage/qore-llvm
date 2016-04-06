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
/// \brief InvokeExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IR_EXPR_INVOKEEXPRESSION_H_
#define INCLUDE_QORE_IR_EXPR_INVOKEEXPRESSION_H_

#include <cassert>
#include <vector>
#include "qore/common/Exceptions.h"
#include "qore/ir/decl/Function.h"
#include "qore/ir/expr/Expression.h"

namespace qore {
namespace ir {

class InvokeExpression : public Expression {

public:
    using Ptr = std::unique_ptr<InvokeExpression>;

public:
    static Ptr create(const Function &function, Expression::Ptr arg1) {
        std::vector<Expression::Ptr> args;
        args.push_back(std::move(arg1));
        return Ptr(new InvokeExpression(function, std::move(args)));
    }

    static Ptr create(const Function &function, Expression::Ptr arg1, Expression::Ptr arg2) {
        std::vector<Expression::Ptr> args;
        args.push_back(std::move(arg1));
        args.push_back(std::move(arg2));
        return Ptr(new InvokeExpression(function, std::move(args)));
    }

    static Ptr create(const Function &function, std::vector<Expression::Ptr> args) {
        return Ptr(new InvokeExpression(function, std::move(args)));
    }

    Kind getKind() const override {
        return Kind::Invoke;
    }

    const Type &getType() const override {
        return function.getRetType();
    }

    const Function &getFunction() const {
        return function;
    }

    std::vector<Expression::Ptr>::size_type getArgCount() const {
        return args.size();
    }

    const Expression &getArg(std::vector<Expression::Ptr>::size_type index) const {
        assert(index >= 0 && index < args.size());
        return *args[index];
    }

private:
    InvokeExpression(const Function &function, std::vector<Expression::Ptr> args)
        : function(function), args(std::move(args)) {
    }

private:
    const Function &function;
    std::vector<Expression::Ptr> args;
};

} // namespace ir
} // namespace qore

#endif // INCLUDE_QORE_IR_EXPR_INVOKEEXPRESSION_H_
