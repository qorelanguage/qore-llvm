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
/// \brief AST node visitor.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_VISITOR_H_
#define INCLUDE_QORE_COMP_AST_VISITOR_H_

namespace qore {
namespace comp {
namespace ast {

class EmptyStatement;
class ExpressionStatement;
class CompoundStatement;
class ReturnStatement;
class IfStatement;
class TryStatement;
class ForeachStatement;
class ThrowStatement;
class SimpleStatement;
class ScopeGuardStatement;
class WhileStatement;
class DoWhileStatement;
class ForStatement;
class SwitchStatement;
class ErrorExpression;
class LiteralExpression;
class NameExpression;
class ListExpression;
class HashExpression;
class VarDeclExpression;
class CastExpression;
class CallExpression;
class UnaryExpression;
class IndexExpression;
class AccessExpression;
class NewExpression;
class BinaryExpression;
class InstanceofExpression;
class ConditionalExpression;
class AssignmentExpression;
class ListOperationExpression;
class RegexExpression;
class ClosureExpression;

/**
 * \brief Interface for expression visitors.
 */
template<typename R>
class ExpressionVisitor {

public:
    virtual ~ExpressionVisitor() {}

    /**
     * \brief Called by an ErrorExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(ErrorExpression &node) = 0;

    /**
     * \brief Called by a LiteralExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(LiteralExpression &node) = 0;

    /**
     * \brief Called by a NameExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(NameExpression &node) = 0;

    /**
     * \brief Called by a ListExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(ListExpression &node) = 0;

    /**
     * \brief Called by a HashExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(HashExpression &node) = 0;

    /**
     * \brief Called by a VarDeclExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(VarDeclExpression &node) = 0;

    /**
     * \brief Called by a CastExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(CastExpression &node) = 0;

    /**
     * \brief Called by a CallExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(CallExpression &node) = 0;

    /**
     * \brief Called by a UnaryExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(UnaryExpression &node) = 0;

    /**
     * \brief Called by an IndexExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(IndexExpression &node) = 0;

    /**
     * \brief Called by an AccessExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(AccessExpression &node) = 0;

    /**
     * \brief Called by a NewExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(NewExpression &node) = 0;

    /**
     * \brief Called by a BinaryExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(BinaryExpression &node) = 0;

    /**
     * \brief Called by a InstanceofExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(InstanceofExpression &node) = 0;

    /**
     * \brief Called by a ConditionalExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(ConditionalExpression &node) = 0;

    /**
     * \brief Called by an AssignmentExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(AssignmentExpression &node) = 0;

    /**
     * \brief Called by an ListOperationExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(ListOperationExpression &node) = 0;

    /**
     * \brief Called by a RegexExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(RegexExpression &node) = 0;

    /**
     * \brief Called by a ClosureExpression AST node.
     * \param node the node being visited
     */
    virtual R visit(ClosureExpression &node) = 0;

protected:
    ExpressionVisitor() = default;

private:
    ExpressionVisitor(const ExpressionVisitor &) = delete;
    ExpressionVisitor(ExpressionVisitor &&) = delete;
    ExpressionVisitor &operator=(const ExpressionVisitor &) = delete;
    ExpressionVisitor &operator=(ExpressionVisitor &&) = delete;
};

/**
 * \brief Interface for statement visitors.
 */
template<typename R>
class StatementVisitor {

public:
    virtual ~StatementVisitor() {}

    /**
     * \brief Called by an EmptyStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(EmptyStatement &node) = 0;

    /**
     * \brief Called by an ExpressionStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(ExpressionStatement &node) = 0;

    /**
     * \brief Called by a CompoundStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(CompoundStatement &node) = 0;

    /**
     * \brief Called by a ReturnStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(ReturnStatement &node) = 0;

    /**
     * \brief Called by an IfStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(IfStatement &node) = 0;

    /**
     * \brief Called by a TryStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(TryStatement &node) = 0;

    /**
     * \brief Called by a ForeachStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(ForeachStatement &node) = 0;

    /**
     * \brief Called by a ThrowStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(ThrowStatement &node) = 0;

    /**
     * \brief Called by a SimpleStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(SimpleStatement &node) = 0;

    /**
     * \brief Called by a ScopeGuardStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(ScopeGuardStatement &node) = 0;

    /**
     * \brief Called by a WhileStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(WhileStatement &node) = 0;

    /**
     * \brief Called by a DoWhileStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(DoWhileStatement &node) = 0;

    /**
     * \brief Called by a ForStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(ForStatement &node) = 0;

    /**
     * \brief Called by a SwitchStatement AST node.
     * \param node the node being visited
     */
    virtual R visit(SwitchStatement &node) = 0;

protected:
    StatementVisitor() = default;

private:
    StatementVisitor(const StatementVisitor &) = delete;
    StatementVisitor(StatementVisitor &&) = delete;
    StatementVisitor &operator=(const StatementVisitor &) = delete;
    StatementVisitor &operator=(StatementVisitor &&) = delete;
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_VISITOR_H_
