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
#ifndef LIB_COMP_SEM_STATEMENTANALYZERPASS2_H_
#define LIB_COMP_SEM_STATEMENTANALYZERPASS2_H_

#include "qore/comp/sem/stmt/Statement.h"
#include "qore/comp/sem/stmt/CompoundStatement.h"
#include "qore/comp/sem/stmt/ExpressionStatement.h"
#include "qore/comp/sem/stmt/GlobalVariableInitializationStatement.h"
#include "qore/comp/sem/stmt/StringLiteralInitializationStatement.h"
#include "qore/comp/sem/stmt/TryStatement.h"
#include "qore/comp/as/is.h"
#include "ExpressionAnalyzerPass2.h"

namespace qore {
namespace comp {
namespace sem {

class StatementAnalyzerPass2 {

public:
    using ReturnType = void;

public:
    StatementAnalyzerPass2(Core &core, Builder &builder) : core(core), builder(builder) {
    }

    void visit(const CompoundStatement &stmt) {
        builder.pushCleanupScope(stmt);
        for (const Statement::Ptr &ptr : stmt.getStatements()) {
            ptr->accept(*this);
        }
        builder.popCleanupScope(stmt);
    }

    void visit(const ExpressionStatement &stmt) {
        ExpressionAnalyzerPass2 a(core, builder);
        stmt.getExpression().accept(a);
    }

    void visit(const GlobalVariableInitializationStatement &stmt) {
        as::Temp temp = builder.getFreeTemp();
        ExpressionAnalyzerPass2 a(core, builder, temp);
        stmt.getExpression().accept(a);
        builder.createMakeGlobal(stmt.getGlobalVariable(), temp);    //throws
        builder.setTempFree(temp);
    }

    void visit(const IfStatement &stmt) {
        builder.pushCleanupScope(stmt);

        as::Temp temp = builder.getFreeTemp();
        ExpressionAnalyzerPass2 a(core, builder, temp);
        stmt.getCondition().accept(a);

        as::Block *cont = nullptr;
        as::Block *trueBlock = builder.createBlock();

        if (!stmt.getFalseBranch()) {
            cont = builder.createBlock();
            builder.createBranch(temp, *trueBlock, *cont);
            builder.setTempFree(temp);
        } else {
            as::Block *falseBlock = builder.createBlock();
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
            as::Temp temp = builder.getFreeTemp();
            {
                ExpressionAnalyzerPass2 a(core, builder, temp);
                stmt.getExpression()->accept(a);
            }
            builder.createRet(temp);
            builder.setTempFree(temp);
        } else {
            builder.createRetVoid();
        }
    }

    void visit(const StringLiteralInitializationStatement &stmt) {
        builder.createMakeStringLiteral(stmt.getStringLiteral(), stmt.getValue());
    }

    void visit(const TryStatement &stmt) {
        as::Block *cont = nullptr;
        as::Block *catchBlock = builder.createBlock();
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

private:
    Core &core;
    Builder &builder;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_STATEMENTANALYZERPASS2_H_
