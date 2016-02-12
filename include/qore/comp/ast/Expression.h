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
#include "qore/comp/ast/Node.h"
#include "qore/comp/ast/Type.h"
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
        return name.getStart();
    }

    SourceLocation getEnd() const override {
        return name.getEnd();
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

/**
 * \brief Represents a variable declaration.
 */
class VarDeclExpression : public Expression {

public:
    Type::Ptr type;                                         //!< The type.
    Token name;                                             //!< The name.

public:
    using Ptr = std::unique_ptr<VarDeclExpression>;         //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param type the type
     * \param name the name
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Type::Ptr type, Token name) {
        return Ptr(new VarDeclExpression(std::move(type), name));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return type->getStart();
    }

    SourceLocation getEnd() const override {
        return name.location;
    }

private:
    explicit VarDeclExpression(Type::Ptr type, Token name) : type(std::move(type)), name(name) {
    }
};

/**
 * \brief Represents a cast expression.
 */
class CastExpression : public Expression {

public:
    SourceLocation start;                                   //!< The starting location.
    Type::Ptr type;                                         //!< The type.
    Expression::Ptr expression;                             //!< The expression.
    SourceLocation end;                                     //!< The ending location.

public:
    using Ptr = std::unique_ptr<CastExpression>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new CastExpression());
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    CastExpression() {
    }
};

/**
 * \brief Represents a list of arguments.
 */
class ArgList : public Node {

public:
    using Data = std::vector<Expression::Ptr>;              //!< The type of the container.

public:
    Token startToken;                                       //!< The opening bracket.
    Data data;                                              //!< The values.
    Token endToken;                                         //!< The closing bracket.

public:
    using Ptr = std::unique_ptr<ArgList>;                   //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param startToken the opening bracket
     * \param data the elements of the list
     * \param endToken the closing bracket
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Token startToken, Data data, Token endToken) {
        return Ptr(new ArgList(startToken, std::move(data), endToken));
    }

    SourceLocation getStart() const override {
        return startToken.location;
    }

    SourceLocation getEnd() const override {
        return endToken.location;
    }

private:
    ArgList(Token startToken, Data data, Token endToken) : startToken(startToken),
            data(std::move(data)), endToken(endToken) {
    }
};

/**
 * \brief Represents a function or method invocation expression.
 */
class CallExpression : public Expression {

public:
    Expression::Ptr calee;                                  //!< The function to be called.
    ArgList::Ptr argList;                                   //!< The list of arguments.

public:
    using Ptr = std::unique_ptr<CallExpression>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param calee the function to be called
     * \param argList the list of arguments
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Expression::Ptr calee, ArgList::Ptr argList) {
        return Ptr(new CallExpression(std::move(calee), std::move(argList)));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return calee->getStart();
    }

    SourceLocation getEnd() const override {
        return argList->getEnd();
    }

private:
    CallExpression(Expression::Ptr calee, ArgList::Ptr argList) : calee(std::move(calee)), argList(std::move(argList)) {
    }
};

/**
 * \brief Represents a unary expression.
 */
class UnaryExpression : public Expression {

public:
    Expression::Ptr expr;                                   //!< The subexpression.
    Token op;                                               //!< The operator.
    bool postfix;                                           //!< True if `op` is a postfix operator.

public:
    using Ptr = std::unique_ptr<UnaryExpression>;           //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param expr the subexpression
     * \param op the operator
     * \param postfix true if `op` is a postfix operator
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Expression::Ptr expr, Token op, bool postfix) {
        return Ptr(new UnaryExpression(std::move(expr), op, postfix));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return postfix ? expr->getStart() : op.location;
    }

    SourceLocation getEnd() const override {
        return postfix ? op.location : expr->getEnd();
    }

private:
    UnaryExpression(Expression::Ptr expr, Token op, bool postfix) : expr(std::move(expr)), op(op), postfix(postfix) {
    }
};

/**
 * \brief Represents an index expression (e.(e), e[e], e{e}).
 */
class IndexExpression : public Expression {

public:
    Expression::Ptr left;                                   //!< The left subexpression.
    Expression::Ptr right;                                  //!< The right subexpression.
    Token endToken;                                         //!< The closing bracket.

public:
    using Ptr = std::unique_ptr<IndexExpression>;           //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param left the left subexpression
     * \param right the right subexpression
     * \param endToken the closing bracket
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Expression::Ptr left, Expression::Ptr right, Token endToken) {
        return Ptr(new IndexExpression(std::move(left), std::move(right), endToken));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return left->getStart();
    }

    SourceLocation getEnd() const override {
        return endToken.location;
    }

private:
    IndexExpression(Expression::Ptr left, Expression::Ptr right, Token endToken) : left(std::move(left)),
            right(std::move(right)), endToken(endToken) {
    }
};

/**
 * \brief Represents a member access expression (e."x", e.x, e.3, ...).
 */
class AccessExpression : public Expression {

public:
    Expression::Ptr expr;                                   //!< The subexpression.
    Token token;                                            //!< The token after the dot.

public:
    using Ptr = std::unique_ptr<AccessExpression>;          //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param expr the subexpression
     * \param token the token after the dot
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Expression::Ptr expr, Token token) {
        return Ptr(new AccessExpression(std::move(expr), token));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return expr->getStart();
    }

    SourceLocation getEnd() const override {
        return token.location;
    }

private:
    AccessExpression(Expression::Ptr expr, Token token) : expr(std::move(expr)), token(token) {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_EXPRESSION_H_
