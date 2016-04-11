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
#include "qore/ir/stmt/GlobalVariableInitializationStatement.h"
#include "qore/ir/stmt/GlobalVariableFinalizationStatement.h"
#include "qore/comp/sem/GlobalScope.h"
#include "qore/comp/sem/ExpressionAnalyzer.h"
#include "qore/comp/sem/ExpressionAnalyzer2.h"
#include "qore/as/as.h"

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

class GlobalScope2 : public GlobalScope {

public:
    GlobalScope2(Context &ctx, ir::Script &script, ir::UserFunction &qinit, ir::UserFunction &qdone)
            : GlobalScope(ctx, script), qinit(qinit), qdone(qdone) {
    }

protected:
    void addGlobalVariableInitializer(const ir::GlobalVariable &gv, ir::Expression::Ptr init) override {
        qinit.add(ir::GlobalVariableInitializationStatement::create(gv, std::move(init)));
        qdone.add(ir::GlobalVariableFinalizationStatement::create(gv));
    }

private:
    ir::UserFunction &qinit;
    ir::UserFunction &qdone;
};

void analyze(Context &ctx, ir::Script &script, ast::Script &s) {
    Analyzer a(ctx);
    ir::UserFunction &qmain = script.createUserFunction("qmain");
    ir::UserFunction &qinit = script.createUserFunction("qinit");
    ir::UserFunction &qdone = script.createUserFunction("qdone");
    GlobalScope2 globalScope(ctx, script, qinit, qdone);
    BlockScopeImpl mainScope(ctx, globalScope, qmain);

    for (auto &decl : s.members) {
        a.doDecl(*decl, globalScope);
    }

    for (auto &stmt : s.statements) {
        a.doStmt(*stmt, mainScope, [&qmain](ir::Statement::Ptr stmt){qmain.add(std::move(stmt));});
    }

    //analyzer.finalize top level scope -> generate lifetime end for locals
}

void analyze2(as::S &sss, ir::Script &script) {
    for (auto &sl : script.getStringLiterals()) {
        assert(sl->getId() == sss.strings.size());
        sss.strings.push_back(sl->getValue());
        //assign ids here?
    }
    sss.globalCount = script.getGlobalVariables().size();

    for (auto &ff : script.getFunctions()) {
        if (ff->getKind() != qore::ir::Function::Kind::User) {
            continue;
        }
        qore::ir::UserFunction &uf = static_cast<qore::ir::UserFunction &>(*ff);

        as::F::Ptr fptr = as::F::create(uf.getName());
        qore::as::F &f = *fptr;
        sss.functions.push_back(std::move(fptr));
        if (uf.getName() == "qmain") {
            sss.qmain = &f;
        } else if (uf.getName() == "qinit") {
            sss.qinit = &f;
        } else if (uf.getName() == "qdone") {
            sss.qdone = &f;
        }

        qore::as::Builder b(f);
        qore::comp::sem::FA fa(f);
        for (auto &s : uf.getStatements()) {
            switch (s->getKind()) {
                case ir::Statement::Kind::GlobalVariableInitialization: {
                    qore::ir::GlobalVariableInitializationStatement &stmt
                            = static_cast<qore::ir::GlobalVariableInitializationStatement &>(*s);

                    as::Id temp = fa.getTemp();
                    qore::comp::sem::ExpressionAnalyzer2 ea2(fa, b);
                    ea2.evaluate(temp, stmt.getExpression());
                    b.createMakeGlobal(stmt.getGlobalVariable().getId(), temp);     //throws
                    fa.doneTemp(temp);
                    break;
                }
                case ir::Statement::Kind::GlobalVariableFinalization: {
                    qore::ir::GlobalVariableFinalizationStatement &stmt
                            = static_cast<qore::ir::GlobalVariableFinalizationStatement &>(*s);
                    as::Id temp = fa.getTemp();
                    b.createFreeGlobal(temp, stmt.getGlobalVariable().getId());
                    if (!stmt.getGlobalVariable().getType().isPrimitive()) {
                        b.createRefDec(temp, nullptr);      //throws
                    }
                    fa.doneTemp(temp);
                    break;
                }
                case ir::Statement::Kind::Expression: {
                    qore::comp::sem::ExpressionAnalyzer2 ea2(fa, b);
                    ea2.evaluateNoValue(static_cast<const qore::ir::ExpressionStatement &>(*s).getExpression());
                    break;
                }
                default:
                    QORE_NOT_IMPLEMENTED("");
            }
        }
        fa.done();
        b.createRetVoid();
        b.dump();
    }
}

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_ANALYZER_H_
