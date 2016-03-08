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

class Namespace;
class Class;
class NamespaceConstant;
class GlobalVariable;
class Function;
class Method;
class ClassConstant;
class MemberGroup;
class Field;
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
class NameType;
class AsteriskType;
class ImplicitType;

/**
 * \brief Interface for expression visitors.
 */
class ExpressionVisitor {

public:
    virtual ~ExpressionVisitor() {}

    /**
     * \brief Called by an ErrorExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(ErrorExpression &node) = 0;

    /**
     * \brief Called by a LiteralExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(LiteralExpression &node) = 0;

    /**
     * \brief Called by a NameExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(NameExpression &node) = 0;

    /**
     * \brief Called by a ListExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(ListExpression &node) = 0;

    /**
     * \brief Called by a HashExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(HashExpression &node) = 0;

    /**
     * \brief Called by a VarDeclExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(VarDeclExpression &node) = 0;

    /**
     * \brief Called by a CastExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(CastExpression &node) = 0;

    /**
     * \brief Called by a CallExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(CallExpression &node) = 0;

    /**
     * \brief Called by a UnaryExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(UnaryExpression &node) = 0;

    /**
     * \brief Called by an IndexExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(IndexExpression &node) = 0;

    /**
     * \brief Called by an AccessExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(AccessExpression &node) = 0;

    /**
     * \brief Called by a NewExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(NewExpression &node) = 0;

    /**
     * \brief Called by a BinaryExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(BinaryExpression &node) = 0;

    /**
     * \brief Called by a InstanceofExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(InstanceofExpression &node) = 0;

    /**
     * \brief Called by a ConditionalExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(ConditionalExpression &node) = 0;

    /**
     * \brief Called by an AssignmentExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(AssignmentExpression &node) = 0;

    /**
     * \brief Called by an ListOperationExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(ListOperationExpression &node) = 0;

    /**
     * \brief Called by a RegexExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(RegexExpression &node) = 0;

    /**
     * \brief Called by a ClosureExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(ClosureExpression &node) = 0;

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
class StatementVisitor {

public:
    virtual ~StatementVisitor() {}

    /**
     * \brief Called by an EmptyStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(EmptyStatement &node) = 0;

    /**
     * \brief Called by an ExpressionStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(ExpressionStatement &node) = 0;

    /**
     * \brief Called by a CompoundStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(CompoundStatement &node) = 0;

    /**
     * \brief Called by a ReturnStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(ReturnStatement &node) = 0;

    /**
     * \brief Called by an IfStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(IfStatement &node) = 0;

    /**
     * \brief Called by a TryStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(TryStatement &node) = 0;

    /**
     * \brief Called by a ForeachStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(ForeachStatement &node) = 0;

    /**
     * \brief Called by a ThrowStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(ThrowStatement &node) = 0;

    /**
     * \brief Called by a SimpleStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(SimpleStatement &node) = 0;

    /**
     * \brief Called by a ScopeGuardStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(ScopeGuardStatement &node) = 0;

    /**
     * \brief Called by a WhileStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(WhileStatement &node) = 0;

    /**
     * \brief Called by a DoWhileStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(DoWhileStatement &node) = 0;

    /**
     * \brief Called by a ForStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(ForStatement &node) = 0;

    /**
     * \brief Called by a SwitchStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(SwitchStatement &node) = 0;

protected:
    StatementVisitor() = default;

private:
    StatementVisitor(const StatementVisitor &) = delete;
    StatementVisitor(StatementVisitor &&) = delete;
    StatementVisitor &operator=(const StatementVisitor &) = delete;
    StatementVisitor &operator=(StatementVisitor &&) = delete;
};

/**
 * \brief Interface for visitors of namespace member declarations (namespaces, classes, globals, constants, functions).
 */
class NamespaceMemberVisitor {

public:
    virtual ~NamespaceMemberVisitor() {}

    /**
     * \brief Called by a Namespace AST node.
     * \param node the node being visited
     */
    virtual void visit(Namespace &node) = 0;

    /**
     * \brief Called by a Class AST node.
     * \param node the node being visited
     */
    virtual void visit(Class &node) = 0;

    /**
     * \brief Called by a NamespaceConstant AST node.
     * \param node the node being visited
     */
    virtual void visit(NamespaceConstant &node) = 0;

    /**
     * \brief Called by a GlobalVariable AST node.
     * \param node the node being visited
     */
    virtual void visit(GlobalVariable &node) = 0;

    /**
     * \brief Called by a Function AST node.
     * \param node the node being visited
     */
    virtual void visit(Function &node) = 0;

protected:
    NamespaceMemberVisitor() = default;

private:
    NamespaceMemberVisitor(const NamespaceMemberVisitor &) = delete;
    NamespaceMemberVisitor(NamespaceMemberVisitor &&) = delete;
    NamespaceMemberVisitor &operator=(const NamespaceMemberVisitor &) = delete;
    NamespaceMemberVisitor &operator=(NamespaceMemberVisitor &&) = delete;
};

/**
 * \brief Interface for visitors of class member declarations (fields, methods, constants).
 */
class ClassMemberVisitor {

public:
    virtual ~ClassMemberVisitor() {}

    /**
     * \brief Called by a Method AST node.
     * \param node the node being visited
     */
    virtual void visit(Method &node) = 0;

    /**
     * \brief Called by a ClassConstant AST node.
     * \param node the node being visited
     */
    virtual void visit(ClassConstant &node) = 0;

    /**
     * \brief Called by a MemberGroup AST node.
     * \param node the node being visited
     */
    virtual void visit(MemberGroup &node) = 0;

    /**
     * \brief Called by a Field AST node.
     * \param node the node being visited
     */
    virtual void visit(Field &node) = 0;

protected:
    ClassMemberVisitor() = default;

private:
    ClassMemberVisitor(const ClassMemberVisitor &) = delete;
    ClassMemberVisitor(ClassMemberVisitor &&) = delete;
    ClassMemberVisitor &operator=(const ClassMemberVisitor &) = delete;
    ClassMemberVisitor &operator=(ClassMemberVisitor &&) = delete;
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_VISITOR_H_
