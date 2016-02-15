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
/// \brief AST nodes related to statements.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_STATEMENT_H_
#define INCLUDE_QORE_COMP_AST_STATEMENT_H_

#include "qore/comp/ast/Expression.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Base class for all nodes representing statements.
 */
class Statement : public Node {

public:
    using Ptr = std::unique_ptr<Statement>;                 //!< Pointer type.

    /**
     * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Node.
     * \param visitor the visitor to call
     */
    virtual void accept(StatementVisitor &visitor) = 0;
};

/**
 * \brief Represents an empty statement.
 */
class EmptyStatement : public Statement {

public:
    SourceLocation location;                                //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<EmptyStatement>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param location the location of the semicolon token
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceLocation location) {
        return Ptr(new EmptyStatement(location));
    }

    void accept(StatementVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return location;
    }

    SourceLocation getEnd() const override {
        return location;
    }

private:
    explicit EmptyStatement(SourceLocation location) : location(location) {
    }
};

/**
 * \brief Represents an expression statement.
 */
class ExpressionStatement : public Statement {

public:
    SourceLocation start;                                   //!< The starting location.
    Expression::Ptr expression;                             //!< The expression.
    SourceLocation end;                                     //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<ExpressionStatement>;       //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ExpressionStatement());
    }

    void accept(StatementVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    ExpressionStatement() {
    }
};

/**
 * \brief Represents a block.
 */
class CompoundStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the opening curly brace.
    SourceLocation end;                                     //!< The location of the closing curly brace.
    std::vector<Statement::Ptr> statements;                 //!< The statements in the block.

public:
    using Ptr = std::unique_ptr<CompoundStatement>;         //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new CompoundStatement());
    }

    void accept(StatementVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    explicit CompoundStatement() {
    }
};

/**
 * \brief Represents a return statement.
 */
class ReturnStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'return' keyword.
    Expression::Ptr expression;                             //!< The optional expression.
    SourceLocation end;                                     //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<ReturnStatement>;           //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ReturnStatement());
    }

    void accept(StatementVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    ReturnStatement() {
    }
};

/**
 * \brief Represents an if statement.
 */
class IfStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'if' keyword.
    Expression::Ptr condition;                              //!< The condition.
    Statement::Ptr stmtTrue;                                //!< The statement to execute if condition is true.
    Statement::Ptr stmtFalse;                               //!< The optional 'else' statement.

public:
    using Ptr = std::unique_ptr<IfStatement>;               //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new IfStatement());
    }

    void accept(StatementVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return stmtFalse ? stmtFalse->getEnd() : stmtTrue->getEnd();
    }

private:
    IfStatement() {
    }
};

/**
 * \brief Represents a try statement.
 */
class TryStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'try' keyword.
    Statement::Ptr stmtTry;                                 //!< The body of the try statement.
    Type::Ptr exceptionType;                                //!< The type of the exception parameter.
    Token exceptionName;                                    //!< The name of the exception parameter.
    Statement::Ptr stmtCatch;                               //!< The body of the catch part.

public:
    using Ptr = std::unique_ptr<TryStatement>;              //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new TryStatement());
    }

    void accept(StatementVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return stmtCatch->getEnd();
    }

private:
    TryStatement() {
    }
};

/**
 * \brief Represents a foreach statement.
 */
class ForeachStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'foreach' keyword.
    Type::Ptr varType;                                      //!< The type of the variable.
    Token varName;                                          //!< The name of the variable.
    Expression::Ptr expr;                                   //!< The expression to iterate.
    Statement::Ptr stmt;                                    //!< The body of the loop.

public:
    using Ptr = std::unique_ptr<ForeachStatement>;          //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ForeachStatement());
    }

    void accept(StatementVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return stmt->getEnd();
    }

private:
    ForeachStatement() {
    }
};

/**
 * \brief Represents a throw statement.
 */
class ThrowStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'throw' keyword.
    std::vector<Expression::Ptr> exprList;                  //!< The list of expressions.
    SourceLocation end;                                     //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<ThrowStatement>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ThrowStatement());
    }

    void accept(StatementVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    ThrowStatement() {
    }
};

/**
 * \brief Represents a statement consisting of a single keyword, e.g. `break;` or `rethrow;`.
 */
class SimpleStatement : public Statement {

public:
    Token keyword;                                          //!< The keyword.
    SourceLocation end;                                     //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<SimpleStatement>;           //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new SimpleStatement());
    }

    void accept(StatementVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return keyword.location;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    SimpleStatement() {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_STATEMENT_H_
