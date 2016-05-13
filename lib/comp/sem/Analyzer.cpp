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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#include "qore/comp/sem/Analyzer.h"
#include <utility>
#include "StatementAnalyzerPass1.h"
#include "StatementAnalyzerPass2.h"

namespace qore {
namespace comp {
namespace sem {

void Analyzer::processPendingDeclarations() {
    /*
     * for each class in class queue:
     *  - resolve and process superclasses
     */
    core.takeClassQueue();

    for (auto it : core.takeGlobalVariableQueue()) {
        it->pass2();
    }

    for (auto it : core.takeFunctionGroupQueue()) {
        it->pass2();
    }

    /*
     * process const-queue (we need their types which means that the initializer expressions need to be compiled
     *      but there may be cycles which need to be detected)
     */

    for (auto f : core.takeFunctionQueue()) {
        f->analyze();
    }
}

Statement::Ptr Analyzer::doPass1(Scope &scope, ast::Statement &stmt) {
    return StatementAnalyzerPass1::analyze(core, scope, stmt);
}

void Analyzer::doPass2(Builder &builder, Statement &stmt) {
    StatementAnalyzerPass2::analyze(core, builder, stmt);
}

std::pair<Function::Ptr, Function::Ptr> Analyzer::analyze(Context &ctx, ast::Script &node) {
    Analyzer analyzer(ctx);

    for (auto &decl : node.members) {
        analyzer.processDeclaration(*decl);
    }
    analyzer.processPendingDeclarations();

    Function::Ptr qMain;
    if (!node.statements.empty()) {
        ast::Routine::Ptr r = ast::Routine::create();
        r->body = ast::CompoundStatement::create();
        r->body->statements = std::move(node.statements);
        r->type = ast::Type::createImplicit(SourceLocation());

        qMain = Function::Ptr(new Function(FunctionType(Type::Nothing), SourceLocation()));
        FunctionScope mainFs(*qMain, analyzer.core, analyzer.root, *r);
        mainFs.analyze();
    }

    Function::Ptr qInit;
    auto initializers = analyzer.takeInitializers();
    if (!initializers.empty()) {
        qInit = Function::Ptr(new Function(FunctionType(Type::Nothing), SourceLocation()));
        FunctionBuilder initBuilder(*qInit);
        for (auto &stmt : initializers) {
            analyzer.doPass2(initBuilder, *stmt);
        }
        initBuilder.createRetVoid();
    }

    return std::make_pair(std::move(qInit), std::move(qMain));
}

} // namespace sem
} // namespace comp
} // namespace qore
