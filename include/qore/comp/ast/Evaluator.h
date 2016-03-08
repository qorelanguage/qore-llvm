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
/// \brief Helper classes for implementing AST node visitors that return values.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_EVALUATOR_H_
#define INCLUDE_QORE_COMP_AST_EVALUATOR_H_

#include "qore/comp/ast/Visitor.h"
#include "qore/comp/ast/Closure.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Helper class for evaluating expression nodes.
 * \tparam V the type of the value
 * \tparam E the type of the evaluator
 */
template<typename V, typename E>
class ExpressionEvaluatorVisitor : public ExpressionVisitor {

public:
    /**
     * \brief Constructor.
     * \param evaluator the evaluator
     */
    explicit ExpressionEvaluatorVisitor(E &evaluator) : evaluator(evaluator) {
    }

    /**
     * \brief Returns the value evaluated for the visited node.
     * \return the value evaluated for the visited node
     */
    V getValue() {
        return std::move(value);
    }

    void visit(ErrorExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(LiteralExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(NameExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(ListExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(HashExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(VarDeclExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(CastExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(CallExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(UnaryExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(IndexExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(AccessExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(NewExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(BinaryExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(InstanceofExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(ConditionalExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(AssignmentExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(ListOperationExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(RegexExpression &node) override {
        value = evaluator.eval(node);
    }

    void visit(ClosureExpression &node) override {
        value = evaluator.eval(node);
    }

private:
    V value;
    E &evaluator;
};

/**
 * \brief Evaluates an expression node using an evaluator.
 *
 * Calls `V eval(X &)` where X is the concrete type of the node.
 * \tparam V the type of the value
 * \tparam E the type of the evaluator
 * \param node the expression node to evaluate
 * \param evaluator the evaluator
 * \return the return value of `V E::eval(X &)`
 */
template<typename V, typename E>
V evaluateExpressionNode(ast::Expression &node, E &evaluator) {
    ExpressionEvaluatorVisitor<V, E> visitor(evaluator);
    node.accept(visitor);
    return visitor.getValue();
}

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_EVALUATOR_H_
