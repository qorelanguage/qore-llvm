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
/// \brief Visitor for the second pass of statement analysis.
///
//------------------------------------------------------------------------------
#ifndef LIB_COMP_SEM_STATEMENTANALYZERPASS2_H_
#define LIB_COMP_SEM_STATEMENTANALYZERPASS2_H_

#include "qore/comp/sem/stmt/Statement.h"
#include "qore/comp/sem/stmt/CompoundStatement.h"
#include "qore/comp/sem/stmt/ExpressionStatement.h"
#include "qore/comp/sem/stmt/GlobalVariableInitializationStatement.h"
#include "qore/comp/sem/stmt/TryStatement.h"
#include "ExpressionAnalyzerPass2.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Translates a temporary representation of a statement to \ref qore::code.
 *
 * Mainly deals with proper reference counting of temporary values and control flow.
 */
class StatementAnalyzerPass2 {

public:
    /**
     * \brief Translates a statement to code.
     * \param core the shared state of the analyzer
     * \param builder code builder
     * \param stmt the statement to translate
     */
    static void analyze(Core &core, Builder &builder, const Statement &stmt) {
        StatementAnalyzerPass2 a(core, builder);
        stmt.accept(a);
    }

    ///\name Implementation of Statement visitor
    ///\{
    using ReturnType = void;

    void visit(const CompoundStatement &stmt) {
        builder.pushCleanupScope(stmt);
        for (const Statement &s : stmt.getStatements()) {
            s.accept(*this);
        }
        builder.popCleanupScope(stmt);
    }

    void visit(const ExpressionStatement &stmt) {
        ExpressionAnalyzerPass2::eval(core, builder, stmt.getExpression());
    }

    void visit(const GlobalVariableInitializationStatement &stmt) {
        code::Temp temp = builder.getFreeTemp();
        ExpressionAnalyzerPass2::eval(core, builder, temp, stmt.getExpression());
        builder.createGlobalInit(stmt.getGlobalVariable(), temp);
        builder.setTempFree(temp);
    }

    void visit(const IfStatement &stmt) {
        builder.pushCleanupScope(stmt);

        code::Temp temp = builder.getFreeTemp();
        ExpressionAnalyzerPass2::eval(core, builder, temp, stmt.getCondition());

        code::Block *cont = nullptr;
        code::Block *trueBlock = builder.createBlock();

        if (!stmt.getFalseBranch()) {
            cont = builder.createBlock();
            builder.createBranch(temp, *trueBlock, *cont);
            builder.setTempFree(temp);
        } else {
            code::Block *falseBlock = builder.createBlock();
            builder.createBranch(temp, *trueBlock, *falseBlock);
            builder.setTempFree(temp);
            builder.setCurrentBlock(falseBlock);
            builder.pushCleanupScope(*stmt.getFalseBranch());
            stmt.getFalseBranch()->accept(*this);
            builder.popCleanupScope(*stmt.getFalseBranch());
            if (!builder.isTerminated()) {
                cont = builder.createBlock();
                builder.createJump(*cont);
            }
        }

        builder.setCurrentBlock(trueBlock);
        builder.pushCleanupScope(stmt.getTrueBranch());
        stmt.getTrueBranch().accept(*this);
        builder.popCleanupScope(stmt.getTrueBranch());
        if (!builder.isTerminated()) {
            if (!cont) {
                cont = builder.createBlock();
            }
            builder.createJump(*cont);
        }

        if (cont) {
            builder.setCurrentBlock(cont);
        }

        builder.popCleanupScope(stmt);
    }

    void visit(const ReturnStatement &stmt) {
        if (stmt.getExpression()) {
            code::Temp temp = builder.getFreeTemp();
            ExpressionAnalyzerPass2::eval(core, builder, temp, *stmt.getExpression());
            if (stmt.getExpression()->getType().isRefCounted()) {
                builder.addCleanup(temp);
            }
            builder.createRet(temp);
            if (stmt.getExpression()->getType().isRefCounted()) {
                builder.doneCleanup(temp);
            }
            builder.setTempFree(temp);
        } else {
            builder.createRetVoid();
        }
    }

    void visit(const TryStatement &stmt) {
        code::Block *cont = nullptr;
        code::Block *catchBlock = builder.createBlock();
        builder.pushCleanupScope(stmt.getTryBody(), catchBlock);
        stmt.getTryBody().accept(*this);
        if (!builder.isTerminated()) {
            cont = builder.createBlock();
            builder.createJump(*cont);
        }
        builder.popCleanupScope(stmt.getTryBody());

        builder.setCurrentBlock(catchBlock);
        builder.pushCleanupScope(stmt.getCatchBody());
        //store 'current exception' to the user variable
        stmt.getCatchBody().accept(*this);
        if (!builder.isTerminated()) {
            if (!cont) {
                cont = builder.createBlock();
            }
            builder.createJump(*cont);
        }
        builder.popCleanupScope(stmt.getCatchBody());

        if (cont) {
            builder.setCurrentBlock(cont);
        }
        //rethrow statement will:
        // - check that it is inside a catch block
        // - restore 'current exception' from the use variable
        // - jump to the next block in builder's cleanupStack (or resume if there is no such block)
    }
    ///\}

private:
    StatementAnalyzerPass2(Core &core, Builder &builder) : core(core), builder(builder) {
    }

private:
    Core &core;
    Builder &builder;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_STATEMENTANALYZERPASS2_H_
