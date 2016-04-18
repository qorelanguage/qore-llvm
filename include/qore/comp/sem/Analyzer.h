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

class QMS : public Scope {      //temporary - simulates the scope of qmain

public:
    explicit QMS(const Scope &rootScope) : rootScope(rootScope) {
    }

    const as::Type &resolveType(ast::Type &node) const override {
        return rootScope.resolveType(node);
    }

    LocalVariable &createLocalVariable(String::Ref name, SourceLocation location, const as::Type &type) override {
        std::unique_ptr<LocalVariable> ptr = util::make_unique<LocalVariable>(type);
        LocalVariable &lv = *ptr;
        locals.push_back(std::move(ptr));
        return lv;
    }

    Symbol resolveSymbol(ast::Name &name) const override {
        return rootScope.resolveSymbol(name);
    }

private:
    const Scope &rootScope;
    std::vector<std::unique_ptr<LocalVariable>> locals;
};


inline as::Script::Ptr analyze(Context &ctx, ast::Script &node) {
    Core analyzer(ctx);
    NamespaceScope root(analyzer);

    for (auto &decl : node.members) {
        root.processDeclaration(*decl);
    }
    analyzer.processPendingDeclarations();

    //once we support functions, synthesize qmain and put it into funcQueue before calling processPendingDeclarations();
    QMS qms(root);
    BlockScopeImpl topLevelScope(qms);

    std::vector<Statement::Ptr> temp;
    for (auto &stmt : node.statements) {
        temp.push_back(analyzer.doPass1(topLevelScope, *stmt));
    }
    temp.push_back(topLevelScope.finalize());

    FunctionBuilder mainBuilder("qmain");
    for (auto &stmt : temp) {
        analyzer.doPass2(mainBuilder, *stmt);
    }
    mainBuilder.createRetVoid();
    mainBuilder.build(analyzer.scriptBuilder);

    //qinit - pass2
    FunctionBuilder initBuilder("qinit");
    auto initializers = analyzer.scriptBuilder.takeInitializers();
    for (auto &stmt : initializers) {
        analyzer.doPass2(initBuilder, *stmt);
    }
    initBuilder.createRetVoid();
    initBuilder.build(analyzer.scriptBuilder);

    return analyzer.scriptBuilder.build();
}

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_ANALYZER_H_
