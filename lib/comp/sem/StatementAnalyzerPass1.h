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
/// \brief Visitor for the first pass of statement analysis.
///
//------------------------------------------------------------------------------
#ifndef LIB_COMP_SEM_STATEMENTANALYZERPASS1_H_
#define LIB_COMP_SEM_STATEMENTANALYZERPASS1_H_

#include <vector>
#include "qore/comp/ast/Statement.h"
#include "qore/comp/sem/stmt/ExpressionStatement.h"
#include "qore/comp/sem/stmt/IfStatement.h"
#include "qore/comp/sem/stmt/ReturnStatement.h"
#include "qore/comp/sem/stmt/TryStatement.h"
#include "ExpressionAnalyzerPass1.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Translates the AST representation of a statement to a temporary representation.
 *
 * Deals mainly with scopes.
 *
 * Implements the visitor for ast::Statement.
 */
class StatementAnalyzerPass1 {

public:
    /**
     * \brief Analyzes a statement.
     * \param core the shared state of the analyzer
     * \param scope the current scope
     * \param stmt the AST node to analyze
     * \return analyzed statement
     */
    static Statement::Ptr analyze(Core &core, Scope &scope, ast::Statement &stmt) {
        StatementAnalyzerPass1 a(core, scope);
        return stmt.accept(a);
    }

    /**
     * \brief Analyzes a statement in a new block scope.
     * \param core the shared state of the analyzer
     * \param scope the current scope
     * \param stmt the AST node to analyze
     * \return analyzed statement
     */
    static Statement::Ptr analyzeWithNewBlock(Core &core, Scope &scope, ast::Statement &stmt) {
        BlockScope inner(scope);
        StatementAnalyzerPass1 a(core, inner);
        return stmt.accept(a);
    }

    ///\name Implementation of ast::Statement visitor
    ///\{
    using ReturnType = Statement::Ptr;

    Statement::Ptr visit(const ast::ExpressionStatement &node) {
        return ExpressionStatement::create(ExpressionAnalyzerPass1::eval(core, scope, *node.expression));
    }

    Statement::Ptr visit(const ast::CompoundStatement &node) {
        std::vector<Statement::Ptr> statements;
        BlockScope inner(scope);
        for (auto &s : node.statements) {
            statements.push_back(analyze(core, inner, *s));
        }
        return CompoundStatement::create(std::move(statements));
    }

    Statement::Ptr visit(const ast::ReturnStatement &node) {
        if (!node.expression) {
            if (scope.getReturnType() != Type::Nothing) {
                QORE_UNREACHABLE("report error");
            }
            return ReturnStatement::create();
        }
        if (scope.getReturnType() == Type::Nothing) {
            QORE_UNREACHABLE("report error");
        }
        return ReturnStatement::create(
                ExpressionAnalyzerPass1::evalAndConvert(core, scope, scope.getReturnType(), *node.expression));
    }

    Statement::Ptr visit(const ast::IfStatement &node) {
        BlockScope inner(scope);
        Expression::Ptr cond = ExpressionAnalyzerPass1::evalAndConvert(core, inner, Type::SoftBool,
                *node.condition);
        Statement::Ptr b1 = analyzeWithNewBlock(core, inner, *node.stmtTrue);
        Statement::Ptr b2 = node.stmtFalse ? analyzeWithNewBlock(core, inner, *node.stmtFalse) : nullptr;
        return IfStatement::create(std::move(cond), std::move(b1), std::move(b2));
    }

    Statement::Ptr visit(const ast::TryStatement &node) {
        Statement::Ptr tryStmt = analyzeWithNewBlock(core, scope, *node.stmtTry);
        Statement::Ptr catchStmt = analyzeWithNewBlock(core, scope, *node.stmtCatch);
        return TryStatement::create(std::move(tryStmt), std::move(catchStmt));
    }

    Statement::Ptr visit(const ast::EmptyStatement &node) { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(const ast::ForeachStatement &node) { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(const ast::ThrowStatement &node) { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(const ast::SimpleStatement &node) { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(const ast::ScopeGuardStatement &node) { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(const ast::WhileStatement &node) { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(const ast::DoWhileStatement &node) { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(const ast::ForStatement &node) { QORE_NOT_IMPLEMENTED(""); }
    Statement::Ptr visit(const ast::SwitchStatement &node) { QORE_NOT_IMPLEMENTED(""); }
    ///\}

private:
    StatementAnalyzerPass1(Core &core, Scope &scope) : core(core), scope(scope) {
    }

private:
    Core &core;
    Scope &scope;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_STATEMENTANALYZERPASS1_H_
