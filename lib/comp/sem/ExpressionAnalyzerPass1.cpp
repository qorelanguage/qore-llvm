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
/// \brief Implements the GlobalVariableInfo class.
///
//------------------------------------------------------------------------------
#include "ExpressionAnalyzerPass1.h"
#include "qore/comp/sem/FunctionGroupInfo.h"
#include <vector>

namespace qore {
namespace comp {
namespace sem {

Expression::Ptr ExpressionAnalyzerPass1::visit(const ast::CallExpression &node) {
    std::vector<Expression::Ptr> args;
    std::vector<const Type *> argTypes;
    for (auto &arg : node.argList->data) {
        Expression::Ptr e = eval(*arg);
        argTypes.push_back(&e->getType());
        args.push_back(std::move(e));
    }
    Expression::Ptr callee = eval(*node.callee);
    if (callee->getKind() == Expression::Kind::FunctionGroup) {
        //FIXME catch exception and report diagnostic
        OverloadResolutionResult overload = static_cast<FunctionGroupExpression &>(*callee).getFunctionGroup()
                .getRt().resolveOverload(argTypes);
        std::vector<const Conversion *> conversions = overload.getArgConversions();
        assert(args.size() == conversions.size());
        for (Index i = 0; i < args.size(); ++i) {
            if (conversions[i]) {
                args[i] = InvokeConversionExpression::create(*conversions[i], std::move(args[i]));
            }
        }
        return FunctionCallExpression::create(overload.getFunction(), std::move(args));
    } else {
        //the type of callee should be either a reference to code, closure, or any
        QORE_NOT_IMPLEMENTED("");
    }
}

} // namespace sem
} // namespace comp
} // namespace qore
