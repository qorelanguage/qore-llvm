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
/// \brief AST nodes related to expressions.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_EXPRESSION_H_
#define INCLUDE_QORE_COMP_AST_EXPRESSION_H_

#include <cassert>
#include "qore/comp/ast/Name.h"
#include "qore/comp/ast/Node.h"
#include "qore/comp/ast/Visitor.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Base class for all nodes representing expressions.
 */
class Expression : public Node {

public:
    using Ptr = std::unique_ptr<Expression>;                //!< Pointer type.

    /**
     * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Node.
     * \param visitor the visitor to call
     */
    virtual void accept(ExpressionVisitor &visitor) = 0;
};

/**
 * \brief Represents an error expression - used for error recovery.
 */
class ErrorExpression : public Expression {

public:
    Token token;                                            //!< The token.

public:
    using Ptr = std::unique_ptr<ErrorExpression>;           //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param token the token
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Token token) {
        return Ptr(new ErrorExpression(token));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return token.location;
    }

    SourceLocation getEnd() const override {
        return token.location;
    }

private:
    explicit ErrorExpression(Token token) : token(token) {
    }
};

/**
 * \brief Represents a literal.
 */
class LiteralExpression : public Expression {

public:
    Token token;                                            //!< The token.

public:
    using Ptr = std::unique_ptr<LiteralExpression>;         //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param token the literal token
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Token token) {
        return Ptr(new LiteralExpression(token));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return token.location;
    }

    SourceLocation getEnd() const override {
        return token.location;
    }

private:
    explicit LiteralExpression(Token token) : token(token) {
    }
};

/**
 * \brief Represents a name.
 */
class NameExpression : public Expression {

public:
    Name name;                                              //!< The name.

public:
    using Ptr = std::unique_ptr<NameExpression>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param name the name
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Name name) {
        return Ptr(new NameExpression(std::move(name)));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return name.tokens[0].location;
    }

    SourceLocation getEnd() const override {
        return name.tokens[name.tokens.size() - 1].location;
    }

private:
    explicit NameExpression(Name name) : name(std::move(name)) {
        assert(!this->name.tokens.empty());
    }
};

/**
 * \brief Represents a list expression.
 */
class ListExpression : public Expression {

public:
    using Data = std::vector<Expression::Ptr>;              //!< The type of the container.

public:
    Token startToken;                                       //!< The opening bracket.
    Data data;                                              //!< The values.
    Token endToken;                                         //!< The closing bracket.

public:
    using Ptr = std::unique_ptr<ListExpression>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param startToken the opening bracket
     * \param data the elements of the list
     * \param endToken the closing bracket
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Token startToken, Data data, Token endToken) {
        return Ptr(new ListExpression(startToken, std::move(data), endToken));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return startToken.location;
    }

    SourceLocation getEnd() const override {
        return endToken.location;
    }

private:
    explicit ListExpression(Token startToken, Data data, Token endToken) : startToken(startToken),
            data(std::move(data)), endToken(endToken) {
    }
};

/**
 * \brief Represents a list expression.
 */
class HashExpression : public Expression {

public:
    using Element = std::pair<Expression::Ptr, Expression::Ptr>;    //!< The element tuple (key, value).
    using Data = std::vector<Element>;                      //!< The type of the container.

public:
    Token startToken;                                       //!< The opening bracket.
    Data data;                                              //!< The values.
    Token endToken;                                         //!< The closing bracket.

public:
    using Ptr = std::unique_ptr<HashExpression>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param startToken the opening bracket
     * \param data the elements of the hash
     * \param endToken the closing bracket
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Token startToken, Data data, Token endToken) {
        return Ptr(new HashExpression(startToken, std::move(data), endToken));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return startToken.location;
    }

    SourceLocation getEnd() const override {
        return endToken.location;
    }

private:
    explicit HashExpression(Token startToken, Data data, Token endToken) : startToken(startToken),
            data(std::move(data)), endToken(endToken) {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_EXPRESSION_H_
