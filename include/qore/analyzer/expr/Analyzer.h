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

namespace qore {
namespace analyzer {
namespace expr {

template<typename Backend, bool needsValue>
class ValueEvaluator;

template<typename Backend>
class Analyzer {

public:
    using Value = typename Backend::Value;
    using LValue = typename Backend::LValue;

public:
    Analyzer(Backend &backend, Scope<Backend> &scope) : backend(backend), scope(scope) {
    }

    LValue evalLValue(const ast::Expression::Ptr &node);
    Value evalValue(const ast::Expression::Ptr &node, LValue dest = LValue());
    void eval(const ast::Expression::Ptr &node, LValue dest = LValue());

    void errLValueExpected(const ast::Node *node) {
        //TODO report error
    }
    LValue createLocalVariable(const ast::VarDecl *node) {
        return scope.createLocalVariable(node->name, node->getRange());
    }
    LValue resolve(const ast::Identifier *node) {
        return scope.resolve(node->name, node->getRange());
    }

private:
    Analyzer(const Analyzer &) = delete;
    Analyzer(Analyzer &&) = delete;
    Analyzer &operator=(const Analyzer &) = delete;
    Analyzer &operator=(Analyzer &&) = delete;

private:
    Backend &backend;
    Scope<Backend> &scope;

    friend class ValueEvaluator<Backend, true>;   //TODO remove friend
    friend class ValueEvaluator<Backend, false>;   //TODO remove friend
};

template<typename Backend>
class LValueEvaluator : public ast::ExpressionVisitor {

public:
    using LValue = typename Backend::LValue;

    LValueEvaluator(Analyzer<Backend> &analyzer) : analyzer(analyzer) {
    }

    void visit(const ast::IntegerLiteral *node) override {
        analyzer.errLValueExpected(node);
    }

    void visit(const ast::StringLiteral *node) override {
        analyzer.errLValueExpected(node);
    }

    void visit(const ast::BinaryExpression *node) override {
        analyzer.errLValueExpected(node);
    }

    void visit(const ast::UnaryExpression *node) override {
        analyzer.errLValueExpected(node);
    }

    void visit(const ast::Assignment *node) override {
        analyzer.errLValueExpected(node);
    }

    void visit(const ast::VarDecl *node) override {
        result = analyzer.createLocalVariable(node);
    }

    void visit(const ast::Identifier *node) override {
        result = analyzer.resolve(node);
    }

public:
    LValue result;

private:
    Analyzer<Backend> &analyzer;
};

template<typename Backend, bool needsValue>
class ValueEvaluator : public ast::ExpressionVisitor {

public:
    using Value = typename Backend::Value;
    using LValue = typename Backend::LValue;

    ValueEvaluator(Analyzer<Backend> &analyzer, LValue dest) : analyzer(analyzer), dest(std::move(dest)) {
    }

    void visit(const ast::IntegerLiteral *node) override {
        //TODO
    }

    void visit(const ast::StringLiteral *node) override {
        assign(analyzer.backend.createStringConstant(node->value, node->getRange()));
        checkNoEffect();
    }

    void visit(const ast::BinaryExpression *node) override {
        Value l = analyzer.evalValue(node->left);
        Value r = analyzer.evalValue(node->right);
        if (!dest) {
            dest = analyzer.backend.createTemporaryVariable(node->operatorRange);
        }
        analyzer.backend.add(dest, std::move(l), std::move(r));
        result = std::move(dest);
        checkNoEffect();
    }

    void visit(const ast::UnaryExpression *node) override {
        Value lValue = analyzer.evalLValue(node->operand);
        analyzer.backend.trim(lValue);
        assign(std::move(lValue));
    }

    void visit(const ast::Assignment *node) override {
        assign(analyzer.evalValue(node->right, analyzer.evalLValue(node->left)));
    }

    void visit(const ast::VarDecl *node) override {
        assign(analyzer.createLocalVariable(node));
    }

    void visit(const ast::Identifier *node) override {
        assign(analyzer.resolve(node));
        checkNoEffect();
    }

private:
    void assign(Value value) {
        if (!dest) {
            result = std::move(value);
        } else {
            analyzer.backend.assign(dest, std::move(value));
            result = std::move(dest);
        }
    }

    void checkNoEffect() {
        if (!dest && !needsValue) {
            //error statement has no effect
        }
    }

public:
    Value result;

private:
    Analyzer<Backend> &analyzer;
    LValue dest;
};

template<typename Backend>
typename Backend::LValue Analyzer<Backend>::evalLValue(const ast::Expression::Ptr &node) {
    LValueEvaluator<Backend> visitor(*this);
    node->accept(visitor);
    return std::move(visitor.result);
}

template<typename Backend>
typename Backend::Value Analyzer<Backend>::evalValue(const ast::Expression::Ptr &node, LValue dest) {
    ValueEvaluator<Backend, true> visitor(*this, std::move(dest));
    node->accept(visitor);
    return std::move(visitor.result);
}

template<typename Backend>
void Analyzer<Backend>::eval(const ast::Expression::Ptr &node, LValue dest) {
    ValueEvaluator<Backend, false> visitor(*this, std::move(dest));
    node->accept(visitor);
}

} // namespace expr
} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_EXPR_ANALYZER_H_
