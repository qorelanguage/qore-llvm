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
#ifndef LIB_COMP_SEM_STATEMENTANALYZERPASS1_H_
#define LIB_COMP_SEM_STATEMENTANALYZERPASS1_H_

#include <vector>
#include "qore/comp/ast/Statement.h"
#include "qore/comp/sem/stmt/ExpressionStatement.h"
#include "qore/comp/sem/stmt/ReturnStatement.h"
#include "ExpressionAnalyzerPass1.h"

namespace qore {
namespace comp {
namespace sem {

class StatementAnalyzerPass1 : public ast::StatementVisitor<Statement::Ptr> {

public:
    StatementAnalyzerPass1(Core &core, Scope &scope) : core(core), scope(scope) {
    }

    Statement::Ptr visit(ast::ExpressionStatement &node) override {
        return ExpressionStatement::create(ExpressionAnalyzerPass1::eval(core, scope, *node.expression));
    }

    Statement::Ptr visit(ast::CompoundStatement &node) override {
        std::vector<Statement::Ptr> statements;
        BlockScopeImpl inner(scope);
        for (auto &s : node.statements) {
            StatementAnalyzerPass1 a(core, inner);
            statements.push_back(s->accept(a));
        }
        statements.push_back(inner.finalize());
        return CompoundStatement::create(std::move(statements));
    }

    Statement::Ptr visit(ast::ReturnStatement &node) override {
        if (!node.expression) {
            if (scope.getReturnType() != as::Type::Nothing) {
                QORE_UNREACHABLE("report error");
            }
            return ReturnStatement::create();
        }
        if (scope.getReturnType() == as::Type::Nothing) {
            QORE_UNREACHABLE("report error");
        }
        return ReturnStatement::create(
                ExpressionAnalyzerPass1::evalAndConvert(core, scope, scope.getReturnType(), *node.expression));
    }

    Statement::Ptr visit(ast::EmptyStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::IfStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::TryStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::ForeachStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::ThrowStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::SimpleStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::ScopeGuardStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::WhileStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::DoWhileStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::ForStatement &node) override { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(ast::SwitchStatement &node) override { QORE_NOT_IMPLEMENTED(""); }

private:
    Core &core;
    Scope &scope;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_STATEMENTANALYZERPASS1_H_
