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
/// \brief AST nodes representing expressions.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_EXPRESSION_H_
#define INCLUDE_QORE_AST_EXPRESSION_H_

#include <memory>
#include "qore/qore.h"
#include "qore/ast/Node.h"

namespace qore {
namespace ast {

/**
 * \brief Common base for all expression nodes.
 */
class Expression : public Node {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<Expression>;
};

/**
 * \brief Represents an integer constant.
 */
class IntegerLiteral : public Expression {

public:
    qint value;         //!< The value of the integer literal.

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<IntegerLiteral>;

public:
    /**
     * \brief Allocates a new node.
     * \param value the value of the integer literal
     * \return a unique pointer to the allocated node
     */
    static Ptr create(int value) {
        return Ptr(new IntegerLiteral(value));
    }

    void *accept(Visitor &v) const override {
        return v.visit(this);
    }

private:
    IntegerLiteral(int value) : value(value) {
    }
};

/**
 * \brief Represents a string literal.
 */
class StringLiteral : public Expression {

public:
    std::string value;      //!< The value of the string literal.

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<StringLiteral>;

public:
    /**
     * \brief Allocates a new node.
     * \param value the value of the string literal
     * \return a unique pointer to the allocated node
     */
    static Ptr create(std::string value) {
        return Ptr(new StringLiteral(value));
    }

    void *accept(Visitor &v) const override {
        return v.visit(this);
    }

private:
    StringLiteral(std::string value) : value(value) {
    }
};

/**
 * \brief Represents a binary expression.
 */
class BinaryExpression : public Expression {

public:
    Expression::Ptr left;           //!< The operand on the left side.
    Expression::Ptr right;          //!< The operand on the right side.

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<BinaryExpression>;

public:
    /**
     * \brief Allocates a new node.
     * \param left the operand on the left side
     * \param right the operand on the right side
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Expression::Ptr left, Expression::Ptr right) {
        return Ptr(new BinaryExpression(std::move(left), std::move(right)));
    }

    void *accept(Visitor &v) const override {
        return v.visit(this);
    }

private:
    BinaryExpression(Expression::Ptr left, Expression::Ptr right) : left(std::move(left)), right(std::move(right)) {
    }
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_EXPRESSION_H_
