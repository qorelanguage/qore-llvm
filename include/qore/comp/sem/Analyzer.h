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
#ifndef INCLUDE_QORE_COMP_SEM_ANALYZER_H_
#define INCLUDE_QORE_COMP_SEM_ANALYZER_H_

#include "qore/ir/stmt/ExpressionStatement.h"
#include "qore/comp/sem/GlobalScope.h"
#include "qore/comp/sem/ExpressionAnalyzer.h"

namespace qore {
namespace comp {
namespace sem {

class Analyzer {

public:
    using StmtConsumer = std::function<void(ir::Statement::Ptr)>;

public:
    explicit Analyzer(qore::comp::Context &ctx) : ctx(ctx) {
    }

    void doDecl(qore::comp::ast::Declaration &decl, qore::comp::sem::GlobalScope &scope) {
        if (decl.getKind() == qore::comp::ast::Declaration::Kind::GlobalVariable) {
            qore::comp::ast::GlobalVariable &gv = static_cast<qore::comp::ast::GlobalVariable &>(decl);
            scope.declareGlobalVariable(gv.name, scope.resolveType(gv.type));
        }
    }

    void doStmt(qore::comp::ast::Statement &stmt, qore::comp::sem::BlockScope &scope, StmtConsumer consumer) {
        qore::comp::sem::ExpressionAnalyzer ea(ctx, scope);
        static_cast<qore::comp::ast::ExpressionStatement &>(stmt).expression->accept(ea);
        consumer(ir::ExpressionStatement::create(std::move(ea.result)));
    }

private:
    qore::comp::Context &ctx;
};

ir::UserFunction &analyze(Context &ctx, ir::Script &script, ast::Script &s) {
    Analyzer a(ctx);
    ir::UserFunction &qmain = script.createUserFunction("qmain");
    GlobalScope globalScope(ctx, script);
    BlockScopeImpl mainScope(ctx, globalScope, qmain);

    for (auto &decl : s.members) {
        a.doDecl(*decl, globalScope);
    }

    for (auto &stmt : s.statements) {
        a.doStmt(*stmt, mainScope, [&qmain](ir::Statement::Ptr stmt){qmain.add(std::move(stmt));});
    }

    //analyzer.finalize top level scope -> generate lifetime end for locals
    return qmain;
}

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_ANALYZER_H_
