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
#ifndef INCLUDE_QORE_ANALYZER_EXPR_ANALYZER_H_
#define INCLUDE_QORE_ANALYZER_EXPR_ANALYZER_H_

#include "qore/analyzer/Scope.h"
#include "qore/ast/Expression.h"
#include "qore/common/Util.h"       //TODO remove

#include "qore/qil/CodeBuilder.h"

namespace qore {
namespace analyzer {
namespace expr {


class LValueEvaluator : public ast::ExpressionVisitor {

public:
    LValueEvaluator(Scope &scope, qil::CodeBuilder &codeBuilder) : scope(scope), codeBuilder(codeBuilder) {
    }

    void visit(const ast::IntegerLiteral *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::StringLiteral *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::BinaryExpression *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::UnaryExpression *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::Assignment *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::VarDecl *node) override {
        codeBuilder.loadVarPtr(node->getRange().start, scope.createLocalVariable(node->name, node->getRange()));
    }

    void visit(const ast::Identifier *node) override {
        codeBuilder.loadVarPtr(node->getRange().start, scope.resolve(node->name, node->getRange()));
    }

private:
    void errLValueExpected(const ast::Node *node) {
        //TODO report error
        QORE_UNREACHABLE("Not implemented");
    }

private:
    Scope &scope;
    qil::CodeBuilder &codeBuilder;
};

template<bool needsValue>
class ValueEvaluator : public ast::ExpressionVisitor {
public:
    ValueEvaluator(Scope &scope, qil::CodeBuilder &codeBuilder) : scope(scope), codeBuilder(codeBuilder) {
    }

    void visit(const ast::IntegerLiteral *node) override {
        QORE_UNREACHABLE("Push int");
        checkNoEffect();
    }

    void visit(const ast::StringLiteral *node) override {
        codeBuilder.pushString(node->getRange().start, scope.createStringLiteral(node->value, node->getRange()));
    }

    void visit(const ast::BinaryExpression *node) override {
        evalValue(node->left);
        evalValue(node->right);
        codeBuilder.add(node->operatorRange.start);
        checkNoEffect();
    }

    void visit(const ast::UnaryExpression *node) override {
        evalLValue(node->operand);
        codeBuilder
            .loadUnique(node->getRange().start)
            .trim(node->getRange().start)
            .dupCond(node->getRange().start, needsValue)
            .assign(node->getRange().start);
    }

    void visit(const ast::Assignment *node) override {
        evalValue(node->right);
        evalLValue(node->left);
        //type conversions & dup - if the type of lhs is known at compile time, we can do it before locking
        codeBuilder
            .dupCond(node->getRange().start, needsValue)
            .assign(node->getRange().start);
    }

    void visit(const ast::VarDecl *node) override {
        scope.createLocalVariable(node->name, node->getRange());
        if (needsValue) {
            codeBuilder.pushNothing(node->getRange().start);
        }
    }

    void visit(const ast::Identifier *node) override {
        codeBuilder.pushVar(node->getRange().start, scope.resolve(node->name, node->getRange()));
        checkNoEffect();
    }

private:
    void checkNoEffect() {
        if (!needsValue) {
            //error statement has no effect
            codeBuilder.discard(SourceLocation());
        }
    }

    void evalLValue(const ast::Expression::Ptr &node) {
        LValueEvaluator visitor(scope, codeBuilder);
        node->accept(visitor);
    }

    void evalValue(const ast::Expression::Ptr &node) {
        ValueEvaluator<true> visitor(scope, codeBuilder);
        node->accept(visitor);
    }

    void eval(const ast::Expression::Ptr &node) {
        ValueEvaluator<false> visitor(scope, codeBuilder);
        node->accept(visitor);
    }


private:
    Scope &scope;
    qil::CodeBuilder &codeBuilder;

};

class Analyzer {

public:
    Analyzer() = default;

    void eval(Scope &scope, qil::CodeBuilder &codeBuilder, const ast::Expression::Ptr &node) {
        ValueEvaluator<false> visitor(scope, codeBuilder);
        node->accept(visitor);
    }

    void evalValue(Scope &scope, qil::CodeBuilder &codeBuilder, const ast::Expression::Ptr &node) {
        ValueEvaluator<true> visitor(scope, codeBuilder);
        node->accept(visitor);
    }

private:
    Analyzer(const Analyzer &) = delete;
    Analyzer(Analyzer &&) = delete;
    Analyzer &operator=(const Analyzer &) = delete;
    Analyzer &operator=(Analyzer &&) = delete;
};

#undef EVAL_LVALUE
#undef EVAL_VALUE
#undef EVAL

} // namespace expr
} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_EXPR_ANALYZER_H_
