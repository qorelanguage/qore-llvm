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
/// \brief TODO File description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_REWRITER_H_
#define INCLUDE_QORE_AST_REWRITER_H_

#include "qore/ast/Expression.h"
#include "qore/ast/Statement.h"
#include "qore/ast/Visitor.h"

namespace qore {
namespace ast {

class Rewriter : public ExpressionVisitor, public StatementVisitor {

protected:
    virtual Expression::Ptr rewrite(IntegerLiteral::Ptr node) {
        return Expression::Ptr();
    }

    virtual Expression::Ptr rewrite(StringLiteral::Ptr node) {
        return Expression::Ptr();
    }

    virtual Expression::Ptr rewrite(BinaryExpression::Ptr node) {
        return Expression::Ptr();
    }

    virtual Expression::Ptr rewrite(UnaryExpression::Ptr node) {
        return Expression::Ptr();
    }

    virtual Expression::Ptr rewrite(Assignment::Ptr node) {
        return Expression::Ptr();
    }

    virtual Expression::Ptr rewrite(VarDecl::Ptr node) {
        return Expression::Ptr();
    }

    virtual Expression::Ptr rewrite(Identifier::Ptr node) {
        return Expression::Ptr();
    }

    virtual Expression::Ptr rewrite(VarRef::Ptr node) {
        return Expression::Ptr();
    }

    virtual Expression::Ptr rewrite(StringConstant::Ptr node) {
        return Expression::Ptr();
    }

    virtual Statement::Ptr rewrite(EmptyStatement::Ptr node) {
        return Statement::Ptr();
    }

    virtual Statement::Ptr rewrite(PrintStatement::Ptr node) {
        return Statement::Ptr();
    }

    virtual Statement::Ptr rewrite(ExpressionStatement::Ptr node) {
        return Statement::Ptr();
    }

    virtual Statement::Ptr rewrite(CompoundStatement::Ptr node) {
        return Statement::Ptr();
    }

    virtual Statement::Ptr rewrite(IfStatement::Ptr node) {
        return Statement::Ptr();
    }

    virtual Statement::Ptr rewrite(TryStatement::Ptr node) {
        return Statement::Ptr();
    }

    virtual Statement::Ptr rewrite(ScopedStatement::Ptr node) {
        return Statement::Ptr();
    }

public:
    void recurse(Expression::Ptr &node) {
        assert(!valueExpr);
        node->accept(*this);
        if (valueExpr) {
            node = valueExpr;
            valueExpr = nullptr;
        }
    }

    void recurse(Statement::Ptr &node) {
        assert(!valueStmt);
        node->accept(*this);
        if (valueStmt) {
            node = valueStmt;
            valueStmt = nullptr;
        }
    }

public:
    Expression::Ptr valueExpr;
    Statement::Ptr valueStmt;

    Rewriter() {
    }

    void visit(IntegerLiteral::Ptr node) override {
        valueExpr = rewrite(node);
    }

    void visit(StringLiteral::Ptr node) override {
        valueExpr = rewrite(node);
    }

    void visit(BinaryExpression::Ptr node) override {
        recurse(node->left);
        recurse(node->right);
        valueExpr = rewrite(node);
    }

    void visit(UnaryExpression::Ptr node) override {
        recurse(node->operand);
        valueExpr = rewrite(node);
    }

    void visit(Assignment::Ptr node) override {
        recurse(node->left);
        recurse(node->right);
        valueExpr = rewrite(node);
    }

    void visit(VarDecl::Ptr node) override {
        valueExpr = rewrite(node);
    }

    void visit(Identifier::Ptr node) override {
        valueExpr = rewrite(node);
    }

    void visit(VarRef::Ptr node) override {
        valueExpr = rewrite(node);
    }

    void visit(StringConstant::Ptr node) override {
        valueExpr = rewrite(node);
    }

    void visit(EmptyStatement::Ptr node) override {
        valueStmt = rewrite(node);
    }

    void visit(PrintStatement::Ptr node) override {
        recurse(node->expression);
        valueStmt = rewrite(node);
    }

    void visit(ExpressionStatement::Ptr node) override {
        recurse(node->expression);
        valueStmt = rewrite(node);
    }

    void visit(CompoundStatement::Ptr node) override {
        for (auto &stmt : node->statements) {
            recurse(stmt);
        }
        valueStmt = rewrite(node);
    }

    void visit(IfStatement::Ptr node) override {
        recurse(node->condition);
        recurse(node->thenBranch);
        recurse(node->elseBranch);
        valueStmt = rewrite(node);
    }

    void visit(TryStatement::Ptr node) override {
        recurse(node->tryBody);
        recurse(node->catchBody);
        valueStmt = rewrite(node);
    }

    void visit(ScopedStatement::Ptr node) override {
        recurse(node->statement);
        valueStmt = rewrite(node);
    }
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_REWRITER_H_
