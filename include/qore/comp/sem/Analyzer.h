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
/// \brief Definition of the semantic analyzer.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_ANALYZER_H_
#define INCLUDE_QORE_COMP_SEM_ANALYZER_H_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "qore/comp/Context.h"
#include "qore/comp/sem/Core.h"
#include "qore/comp/ast/Declaration.h"
#include "qore/comp/sem/NamespaceScope.h"
#include "qore/comp/sem/stmt/Statement.h"
#include "qore/comp/sem/Builder.h"
#include "qore/comp/ast/Script.h"
#include "qore/core/Env.h"

namespace qore {
namespace comp {
namespace sem {

inline std::pair<Function *, Function *> analyze(Context &ctx, Env &rtEnv, ast::Script &node) {
    Core analyzer(ctx);
    NamespaceScope root(analyzer, rtEnv.getRootNamespace());

    for (auto &decl : node.members) {
        root.processDeclaration(*decl);
    }
    analyzer.processPendingDeclarations();

    Function *qMain = nullptr;
    if (!node.statements.empty()) {
        ast::Routine::Ptr r = ast::Routine::create();
        r->body = ast::CompoundStatement::create();
        r->body->statements = std::move(node.statements);
        r->type = ast::Type::createImplicit(SourceLocation());

        qMain = &rtEnv.getRootNamespace().addFunctionGroup("<qmain>")
                .addFunction(FunctionType(Type::Nothing), SourceLocation());
        FunctionScope mainFs(*qMain, analyzer, root, *r);
        mainFs.analyze();
    }

    Function *qInit = nullptr;
    //qinit - pass2
    auto initializers = analyzer.takeInitializers();
    if (!initializers.empty()) {
        qInit = &rtEnv.getRootNamespace().addFunctionGroup("<qinit>")
                .addFunction(FunctionType(Type::Nothing), SourceLocation());
        FBuilder initBuilder(*qInit, ctx);
        for (auto &stmt : initializers) {
            analyzer.doPass2(initBuilder, *stmt);
        }
        initBuilder.createRetVoid();
    }

    return std::make_pair(qInit, qMain);
}

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_ANALYZER_H_
