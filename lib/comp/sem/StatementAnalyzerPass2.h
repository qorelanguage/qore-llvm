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
#include "qore/comp/sem/stmt/LifetimeEndStatement.h"
#include "qore/comp/sem/stmt/StringLiteralInitializationStatement.h"
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
        for (const Statement::Ptr &ptr : stmt.getStatements()) {
            ptr->accept(*this);
        }
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

    void visit(const LifetimeEndStatement &stmt) {
        as::LocalSlot slot = builder.unassignLocalSlot(stmt.getLocalVariable());
        if (!stmt.getLocalVariable().getType().isPrimitive()) {
            builder.doneCleanup(stmt.getLocalVariable());
            as::Temp temp = builder.getFreeTemp();
            builder.createGetLocal(temp, slot);
            builder.createRefDec(temp);
            builder.setTempFree(temp);
        }
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

private:
    Core &core;
    Builder &builder;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_STATEMENTANALYZERPASS2_H_
