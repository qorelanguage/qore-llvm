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

#include <qore/comp/sem/Core.h>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "qore/comp/Context.h"
#include "qore/comp/as/Script.h"
#include "qore/comp/ast/Declaration.h"
#include "qore/comp/sem/NamespaceScope.h"
#include "qore/comp/sem/stmt/Statement.h"
#include "qore/comp/sem/Builder.h"
#include "qore/comp/ast/Script.h"

namespace qore {
namespace comp {
namespace sem {

inline as::Script::Ptr analyze(Context &ctx, ast::Script &node) {
    Core analyzer(ctx);
    NamespaceScope root(analyzer);

    for (auto &decl : node.members) {
        root.processDeclaration(*decl);
    }
    analyzer.processPendingDeclarations();

    ast::Routine::Ptr r = ast::Routine::create();
    r->body = ast::CompoundStatement::create();
    r->body->statements = std::move(node.statements);
    r->type = ast::Type::createImplicit(SourceLocation());

    FunctionScope mainFs(analyzer, root, "qmain", *r);
    as::Function &qMain = mainFs.analyze();

    //qinit - pass2
    Builder initBuilder;
    auto initializers = analyzer.scriptBuilder.takeInitializers();
    for (auto &stmt : initializers) {
        analyzer.doPass2(initBuilder, *stmt);
    }
    initBuilder.createRetVoid();
    as::Function &qInit = analyzer.scriptBuilder.createFunction("qinit", 0, as::Type::Nothing, initBuilder);

    return analyzer.scriptBuilder.build(&qInit, &qMain);
}

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_ANALYZER_H_
