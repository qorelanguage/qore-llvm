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

class Script;
class Namespace;
class Function;
class EmptyStatement;
class ExpressionStatement;
class CompoundStatement;
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

protected:
    StatementVisitor() = default;

private:
    StatementVisitor(const StatementVisitor &) = delete;
    StatementVisitor(StatementVisitor &&) = delete;
    StatementVisitor &operator=(const StatementVisitor &) = delete;
    StatementVisitor &operator=(StatementVisitor &&) = delete;
};

/**
 * \brief Interface for visitors of declarations (namespaces, classes, globals, constants, functions).
 */
class DeclarationVisitor {

public:
    virtual ~DeclarationVisitor() {}

    /**
     * \brief Called by a Script AST node.
     * \param node the node being visited
     */
    virtual void visit(Script &node) = 0;

    /**
     * \brief Called by a Namespace AST node.
     * \param node the node being visited
     */
    virtual void visit(Namespace &node) = 0;

    /**
     * \brief Called by a Function AST node.
     * \param node the node being visited
     */
    virtual void visit(Function &node) = 0;

protected:
    DeclarationVisitor() = default;

private:
    DeclarationVisitor(const DeclarationVisitor &) = delete;
    DeclarationVisitor(DeclarationVisitor &&) = delete;
    DeclarationVisitor &operator=(const DeclarationVisitor &) = delete;
    DeclarationVisitor &operator=(DeclarationVisitor &&) = delete;
};

/**
 * \brief Interface for visitors of types.
 */
class TypeVisitor {

public:
    virtual ~TypeVisitor() {}

    /**
     * \brief Called by a NameType AST node.
     * \param node the node being visited
     */
    virtual void visit(NameType &node) = 0;

    /**
     * \brief Called by an AsteriskType AST node.
     * \param node the node being visited
     */
    virtual void visit(AsteriskType &node) = 0;

    /**
     * \brief Called by an ImplicitType AST node.
     * \param node the node being visited
     */
    virtual void visit(ImplicitType &node) = 0;

protected:
    TypeVisitor() = default;

private:
    TypeVisitor(const TypeVisitor &) = delete;
    TypeVisitor(TypeVisitor &&) = delete;
    TypeVisitor &operator=(const TypeVisitor &) = delete;
    TypeVisitor &operator=(TypeVisitor &&) = delete;
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_VISITOR_H_
