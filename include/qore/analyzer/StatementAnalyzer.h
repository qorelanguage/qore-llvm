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
#ifndef INCLUDE_QORE_ANALYZER_STATEMENTANALYZER_H_
#define INCLUDE_QORE_ANALYZER_STATEMENTANALYZER_H_

#include "qore/analyzer/ExpressionAnalyzer.h"
#include "qore/ast/Program.h"
#include "qore/il/Builder.h"

namespace qore {
namespace analyzer {

template<typename Backend>
class StatementAnalyzer : public ast::StatementVisitor, public ast::ProgramVisitor {

public:
    StatementAnalyzer(Backend &backend, ExpressionAnalyzer<Backend> &expressionAnalyzer) : backend(backend), expressionAnalyzer(expressionAnalyzer) {
    }

    void visit(const ast::EmptyStatement *node) override {
    }

    void visit(const ast::ExpressionStatement *node) override {
        expressionAnalyzer.eval(node->expression);
    }

    void visit(const ast::PrintStatement *node) override {
        backend.print(expressionAnalyzer.evalValue(node->expression));
    }

    void visit(const ast::Program *node) override {
        for (const auto &stmt : node->body) {
            stmt->accept(*this);
        }
    }

private:
    Backend &backend;
    ExpressionAnalyzer<Backend> &expressionAnalyzer;
};

} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_STATEMENTANALYZER_H_
