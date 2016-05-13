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

#include "qore/comp/sem/Builder.h"
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
        LocalsStackMarker marker(builder);
        for (const Statement &s : stmt.getStatements()) {
            s.accept(*this);
        }
    }

    void visit(const ExpressionStatement &stmt) {
        ExpressionAnalyzerPass2::eval(core, builder, stmt.getExpression());
    }

    void visit(const GlobalVariableInitializationStatement &stmt) {
        TempHelper temp(builder);
        ExpressionAnalyzerPass2::eval(core, builder, temp, stmt.getExpression());
        builder.createGlobalInit(stmt.getGlobalVariable(), temp);
    }

    void visit(const IfStatement &stmt) {
        code::Block *trueBlock = builder.createBlock();
        code::Block *falseBlock = builder.createBlock();
        {
            TempHelper temp(builder);
            ExpressionAnalyzerPass2::eval(core, builder, temp, stmt.getCondition());
            builder.createBranch(temp, *trueBlock, *falseBlock);
        }

        code::Block *contBlock = nullptr;
        if (!stmt.getFalseBranch()) {
            contBlock = falseBlock;
        } else {
            builder.setCurrentBlock(falseBlock);
            stmt.getFalseBranch()->accept(*this);
            if (!builder.isTerminated()) {
                contBlock = builder.createBlock();
                builder.createJump(*contBlock);
            }
        }

        builder.setCurrentBlock(trueBlock);
        stmt.getTrueBranch().accept(*this);
        if (!builder.isTerminated()) {
            if (!contBlock) {
                contBlock = builder.createBlock();
            }
            builder.createJump(*contBlock);
        }

        if (contBlock) {
            builder.setCurrentBlock(contBlock);
        }
    }

    void visit(const ReturnStatement &stmt) {
        if (stmt.getExpression()) {
            TempHelper temp(builder);
            ExpressionAnalyzerPass2::eval(core, builder, temp, *stmt.getExpression());
            temp.derefNeeded(stmt.getExpression()->getType().isRefCounted());
            builder.createRet(temp);
            temp.derefDone();
        } else {
            builder.createRetVoid();
        }
    }

    void visit(const TryStatement &stmt) {
        code::Block *contBlock = nullptr;
        code::Block *catchBlock = builder.createBlock();
        stmt.getTryBody().accept(*this);
        if (!builder.isTerminated()) {
            contBlock = builder.createBlock();
            builder.createJump(*contBlock);
        }

        builder.setCurrentBlock(catchBlock);
        //store 'current exception' to the user variable
        stmt.getCatchBody().accept(*this);
        if (!builder.isTerminated()) {
            if (!contBlock) {
                contBlock = builder.createBlock();
            }
            builder.createJump(*contBlock);
        }

        if (contBlock) {
            builder.setCurrentBlock(contBlock);
        }
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
