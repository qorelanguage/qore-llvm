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
    virtual Expression *rewrite(IntegerLiteral *node) {
        return node;
    }

    virtual Expression *rewrite(StringLiteral *node) {
        return node;
    }

    virtual Expression *rewrite(BinaryExpression *node) {
        return node;
    }

    virtual Expression *rewrite(UnaryExpression *node) {
        return node;
    }

    virtual Expression *rewrite(Assignment *node) {
        return node;
    }

    virtual Expression *rewrite(VarDecl *node) {
        return node;
    }

    virtual Expression *rewrite(Identifier *node) {
        return node;
    }

    virtual Expression *rewrite(VarRef *node) {
        return node;
    }

    virtual Expression *rewrite(StrRef *node) {
        return node;
    }

    virtual Statement *rewrite(EmptyStatement *node) {
        return node;
    }

    virtual Statement *rewrite(PrintStatement *node) {
        return node;
    }

    virtual Statement *rewrite(ExpressionStatement *node) {
        return node;
    }

    virtual Statement *rewrite(CompoundStatement *node) {
        return node;
    }

    virtual Statement *rewrite(IfStatement *node) {
        return node;
    }

    virtual Statement *rewrite(TryStatement *node) {
        return node;
    }

    virtual Statement *rewrite(ScopedStatement *node) {
        return node;
    }

public:
    template<typename N>
    void recurse(std::unique_ptr<N> &node) {
        value = nullptr;
        node->accept(*this);
        if (value && node.get() != value) {
            node.release();
            node.reset(static_cast<N*>(value));
        }
    }

public:
    Node *value;

    Rewriter() : value(nullptr) {
    }

    void visit(IntegerLiteral *node) override {
        value = rewrite(node);
    }

    void visit(StringLiteral *node) override {
        value = rewrite(node);
    }

    void visit(BinaryExpression *node) override {
        recurse(node->left);
        recurse(node->right);
        value = rewrite(node);
    }

    void visit(UnaryExpression *node) override {
        recurse(node->operand);
        value = rewrite(node);
    }

    void visit(Assignment *node) override {
        recurse(node->left);
        recurse(node->right);
        value = rewrite(node);
    }

    void visit(VarDecl *node) override {
        value = rewrite(node);
    }

    void visit(Identifier *node) override {
        value = rewrite(node);
    }

    void visit(VarRef *node) override {
        value = rewrite(node);
    }

    void visit(StrRef *node) override {
        value = rewrite(node);
    }

    void visit(EmptyStatement *node) override {
        value = rewrite(node);
    }

    void visit(PrintStatement *node) override {
        recurse(node->expression);
        value = rewrite(node);
    }

    void visit(ExpressionStatement *node) override {
        recurse(node->expression);
        value = rewrite(node);
    }

    void visit(CompoundStatement *node) override {
        for (auto &stmt : node->statements) {
            recurse(stmt);
        }
        value = rewrite(node);
    }

    void visit(IfStatement *node) override {
        recurse(node->condition);
        recurse(node->thenBranch);
        recurse(node->elseBranch);
        value = rewrite(node);
    }

    void visit(TryStatement *node) override {
        recurse(node->tryBody);
        recurse(node->catchBody);
        value = rewrite(node);
    }

    void visit(ScopedStatement *node) override {
        recurse(node->statement);
        value = rewrite(node);
    }
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_REWRITER_H_
