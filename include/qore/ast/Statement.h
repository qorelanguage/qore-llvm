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
/// \brief AST nodes representing statements.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_STATEMENT_H_
#define INCLUDE_QORE_AST_STATEMENT_H_

#include <memory>
#include <vector>
#include "qore/ast/Expression.h"

namespace qore {
namespace ast {

/**
 * \brief Common base for all statement nodes.
 */
class Statement : public Node {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::shared_ptr<Statement>;

    /**
     * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Node.
     * \param visitor the visitor to call
     */
    virtual void accept(StatementVisitor &visitor) = 0;
};

/**
 * \brief A collection of statements.
 */
using Statements = std::vector<Statement::Ptr>;

/**
 * \brief Represents an empty statement.
 */
class EmptyStatement : public Statement, public std::enable_shared_from_this<EmptyStatement> {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::shared_ptr<EmptyStatement>;

public:
    /**
     * \brief Allocates a new node.
     * \param range the location in the source code
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range) {
        return Ptr(new EmptyStatement(range));
    }

    void accept(StatementVisitor &v) override {
        v.visit(shared_from_this());
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    EmptyStatement(SourceRange range) : range(range) {
    }

private:
    SourceRange range;
};

/**
 * \brief Represents a print statement.
 */
class PrintStatement : public Statement, public std::enable_shared_from_this<PrintStatement> {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::shared_ptr<PrintStatement>;

public:
    Expression::Ptr expression;         //!< The expression to print.

public:
    /**
     * \brief Allocates a new node.
     * \param range the location in the source code
     * \param expression the expression to print
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range, Expression::Ptr expression) {
        return Ptr(new PrintStatement(range, std::move(expression)));
    }

    void accept(StatementVisitor &v) override {
        v.visit(shared_from_this());
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    PrintStatement(SourceRange range, Expression::Ptr expression) : expression(std::move(expression)), range(range) {
    }

private:
    SourceRange range;
};

/**
 * \brief Represents an expression statement.
 */
class ExpressionStatement : public Statement, public std::enable_shared_from_this<ExpressionStatement> {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::shared_ptr<ExpressionStatement>;

public:
    Expression::Ptr expression;         //!< The expression.

public:
    /**
     * \brief Allocates a new node.
     * \param range the location in the source code
     * \param expression the expression
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range, Expression::Ptr expression) {
        return Ptr(new ExpressionStatement(range, std::move(expression)));
    }

    void accept(StatementVisitor &v) override {
        v.visit(shared_from_this());
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    ExpressionStatement(SourceRange range, Expression::Ptr expression)
        : expression(std::move(expression)), range(range) {
    }

private:
    SourceRange range;
};

/**
 * \brief Represents a compound statement.
 */
class CompoundStatement : public Statement, public std::enable_shared_from_this<CompoundStatement> {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::shared_ptr<CompoundStatement>;

public:
    Statements statements;          //!< The statements.

public:
    /**
     * \brief Allocates a new node.
     * \param range the location in the source code
     * \param statements the statements
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range, Statements statements) {
        return Ptr(new CompoundStatement(range, std::move(statements)));
    }

    void accept(StatementVisitor &v) override {
        v.visit(shared_from_this());
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    CompoundStatement(SourceRange range, Statements statements) : statements(std::move(statements)), range(range) {
    }

private:
    SourceRange range;
};

class ScopedStatement : public Statement, public std::enable_shared_from_this<ScopedStatement> {
public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::shared_ptr<ScopedStatement>;

public:
    Statement::Ptr statement;

    std::vector<Variable::Ptr> variables;

public:
    /**
     * \brief Allocates a new node.
     * \param statement the statement
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Statement::Ptr statement) {
        return Ptr(new ScopedStatement(std::move(statement)));
    }

    void accept(StatementVisitor &v) override {
        v.visit(shared_from_this());
    }

    SourceRange getRange() const override {
        return statement->getRange();
    }

private:
    ScopedStatement(Statement::Ptr statement) : statement(std::move(statement)) {
    }
};

/**
 * \brief Represents an if-then-else statement.
 */
class IfStatement : public Statement, public std::enable_shared_from_this<IfStatement> {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::shared_ptr<IfStatement>;

public:
    Expression::Ptr condition;              //!< The condition.
    Statement::Ptr thenBranch;              //!< The statements of the positive branch.
    Statement::Ptr elseBranch;              //!< The statements of the negative branch.

public:
    /**
     * \brief Allocates a new node.
     * \param range the location in the source code
     * \param condition the condition
     * \param thenBranch the statements of the positive branch
     * \param elseBranch the statements of the negative branch
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range, Expression::Ptr condition, Statement::Ptr thenBranch,
            Statement::Ptr elseBranch) {
        return Ptr(new IfStatement(range, std::move(condition), std::move(thenBranch), std::move(elseBranch)));
    }

    void accept(StatementVisitor &v) override {
        v.visit(shared_from_this());
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    IfStatement(SourceRange range, Expression::Ptr condition, Statement::Ptr thenBranch, Statement::Ptr elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)), range(range) {
    }

private:
    SourceRange range;
};

/**
 * \brief Represents a try-catch statement.
 */
class TryStatement : public Statement, public std::enable_shared_from_this<TryStatement> {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::shared_ptr<TryStatement>;

public:
    Statement::Ptr tryBody;                 //!< The statements of the try block.
    Identifier::Ptr var;                    //!< The variable in the catch block (can be nullptr)
    Statement::Ptr catchBody;               //!< The statements of the catch block.

public:
    /**
     * \brief Allocates a new node.
     * \param range the location in the source code
     * \param tryBody the statements of the try block
     * \param catchBody the statements of the catch block
     * \param var the variable in the catch block (can be nullptr)
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range, Statement::Ptr tryBody, Identifier::Ptr var, Statement::Ptr catchBody) {
        return Ptr(new TryStatement(range, std::move(tryBody), std::move(var), std::move(catchBody)));
    }

    void accept(StatementVisitor &v) override {
        v.visit(shared_from_this());
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    TryStatement(SourceRange range, Statement::Ptr tryBody, Identifier::Ptr var, Statement::Ptr catchBody)
        : tryBody(std::move(tryBody)), var(std::move(var)), catchBody(std::move(catchBody)), range(range) {
    }

private:
    SourceRange range;
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_STATEMENT_H_
