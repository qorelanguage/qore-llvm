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
//  FITNESS by a PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE by aNY CLAIM, DAMAGES OR OTHER
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
#ifndef INCLUDE_QORE_AST_VISITOR_H_
#define INCLUDE_QORE_AST_VISITOR_H_

namespace qore {
namespace ast {

class IntegerLiteral;
class StringLiteral;
class BinaryExpression;
class UnaryExpression;
class Assignment;
class VarDecl;
class Identifier;
class Variable;
class EmptyStatement;
class PrintStatement;
class ExpressionStatement;
class CompoundStatement;
class IfStatement;
class TryStatement;
class ScopedStatement;
class Program;

/**
 * \brief Interface for expression visitors.
 */
class ExpressionVisitor {

public:
    virtual ~ExpressionVisitor() {}

    /**
     * \brief Called by an IntegerLiteral AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<IntegerLiteral> node) = 0;

    /**
     * \brief Called by a StringLiteral AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<StringLiteral> node) = 0;

    /**
     * \brief Called by a BinaryExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<BinaryExpression> node) = 0;

    /**
     * \brief Called by a UnaryExpression AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<UnaryExpression> node) = 0;

    /**
     * \brief Called by an Assignment AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<Assignment> node) = 0;

    /**
     * \brief Called by a VarDecl AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<VarDecl> node) = 0;

    /**
     * \brief Called by an Identifier AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<Identifier> node) = 0;

    /**
     * \brief Called by a Variable AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<Variable> node) = 0;

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
    virtual void visit(std::shared_ptr<EmptyStatement> node) = 0;

    /**
     * \brief Called by a PrintStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<PrintStatement> node) = 0;

    /**
     * \brief Called by an ExpressionStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<ExpressionStatement> node) = 0;

    /**
     * \brief Called by a CompoundStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<CompoundStatement> node) = 0;

    /**
     * \brief Called by an IfStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<IfStatement> node) = 0;

    /**
     * \brief Called by a TryStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<TryStatement> node) = 0;

    /**
     * \brief Called by a ScopedStatement AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<ScopedStatement> node) = 0;

protected:
    StatementVisitor() = default;

private:
    StatementVisitor(const StatementVisitor &) = delete;
    StatementVisitor(StatementVisitor &&) = delete;
    StatementVisitor &operator=(const StatementVisitor &) = delete;
    StatementVisitor &operator=(StatementVisitor &&) = delete;
};

/**
 * \brief Interface for program visitors.
 */
class ProgramVisitor {

public:
    virtual ~ProgramVisitor() {}

    /**
     * \brief Called by a Program AST node.
     * \param node the node being visited
     */
    virtual void visit(std::shared_ptr<Program> node) = 0;

protected:
    ProgramVisitor() = default;

private:
    ProgramVisitor(const ProgramVisitor &) = delete;
    ProgramVisitor(ProgramVisitor &&) = delete;
    ProgramVisitor &operator=(const ProgramVisitor &) = delete;
    ProgramVisitor &operator=(ProgramVisitor &&) = delete;
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_VISITOR_H_
