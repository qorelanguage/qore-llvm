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
/// \brief TODO File description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_ANALYZER_EXPRESSIONANALYZER_H_
#define INCLUDE_QORE_ANALYZER_EXPRESSIONANALYZER_H_

#include "expr/Analyzer.h"

namespace qore {
namespace analyzer {

template<typename Backend>
class ExpressionAnalyzer {
public:
    using Value = typename Backend::Value;
    using LValue = typename Backend::LValue;

public:
    ExpressionAnalyzer(Backend &builder, Scope<Backend> &scope) : impl(builder, scope) {
    }

    LValue evalLValue(const ast::Expression::Ptr &node) {
        return impl.evalLValue(node);
    }

    Value evalValue(const ast::Expression::Ptr &node) {
        return impl.evalValue(node);
    }

    void eval(const ast::Expression::Ptr &node) {
        impl.eval(node);
    }

private:
    ExpressionAnalyzer(const ExpressionAnalyzer &) = delete;
    ExpressionAnalyzer(ExpressionAnalyzer &&) = delete;
    ExpressionAnalyzer &operator=(const ExpressionAnalyzer &) = delete;
    ExpressionAnalyzer &operator=(ExpressionAnalyzer &&) = delete;

private:
    expr::Analyzer<Backend> impl;
};

} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_EXPRESSIONANALYZER_H_
