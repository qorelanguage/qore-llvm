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
#include "qore/qil/StringLiteral.h"
#include "qore/qil/Variable.h"

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

    /**
     * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Node.
     * \param visitor the visitor to call
     */
    virtual void accept(ExpressionVisitor &visitor) = 0;
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
     * \param range the location of the literal
     * \param value the value of the integer literal
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range, int value) {
        return Ptr(new IntegerLiteral(value, range));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(this);
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    IntegerLiteral(int value, SourceRange range) : value(value), range(range) {
    }

private:
    SourceRange range;
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
     * \param range the location of the literal
     * \param value the value of the string literal
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range, std::string value) {
        return Ptr(new StringLiteral(value, range));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(this);
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    StringLiteral(std::string value, SourceRange range) : value(value), range(range) {
    }

private:
    SourceRange range;
};

/**
 * \brief Represents a binary expression.
 */
class BinaryExpression : public Expression {

public:
    Expression::Ptr left;           //!< The operand on the left side.
    SourceRange operatorRange;      //!< Location of the binary operator in the source.
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
     * \param operatorRange the location of the operator
     * \param right the operand on the right side
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Expression::Ptr left, SourceRange operatorRange, Expression::Ptr right) {
        return Ptr(new BinaryExpression(std::move(left), operatorRange, std::move(right)));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(this);
    }

    SourceRange getRange() const override {
        return SourceRange(left->getRange().start, right->getRange().end);
    }

private:
    BinaryExpression(Expression::Ptr left, SourceRange operatorRange, Expression::Ptr right)
        : left(std::move(left)), operatorRange(operatorRange), right(std::move(right)) {
    }
};

/**
 * \brief Represents a unary expression.
 */
class UnaryExpression : public Expression {

public:
    SourceRange operatorRange;      //!< Location of the operator in the source.
    Expression::Ptr operand;        //!< The operand.

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<UnaryExpression>;

public:
    /**
     * \brief Allocates a new node.
     * \param operatorRange the location of the operator
     * \param operand the operand
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange operatorRange, Expression::Ptr operand) {
        return Ptr(new UnaryExpression(operatorRange, std::move(operand)));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(this);
    }

    SourceRange getRange() const override {
        return SourceRange(operatorRange.start, operand->getRange().end);
    }

private:
    UnaryExpression(SourceRange operatorRange, Expression::Ptr operand)
        : operatorRange(operatorRange), operand(std::move(operand)) {
    }
};

/**
 * \brief Represents an assignment.
 */
class Assignment : public Expression {

public:
    Expression::Ptr left;           //!< The operand on the left side.
    SourceRange operatorRange;      //!< Location of the assignment operator in the source.
    Expression::Ptr right;          //!< The operand on the right side.

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<Assignment>;

public:
    /**
     * \brief Allocates a new node.
     * \param left the operand on the left side
     * \param operatorRange the location of the operator
     * \param right the operand on the right side
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Expression::Ptr left, SourceRange operatorRange, Expression::Ptr right) {
        return Ptr(new Assignment(std::move(left), operatorRange, std::move(right)));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(this);
    }

    SourceRange getRange() const override {
        return SourceRange(left->getRange().start, right->getRange().end);
    }

private:
    Assignment(Expression::Ptr left, SourceRange operatorRange, Expression::Ptr right)
        : left(std::move(left)), operatorRange(operatorRange), right(std::move(right)) {
    }
};

/**
 * \brief Represents a variable declaration.
 */
class VarDecl : public Expression {

public:
    std::string name;               //!< The name of the variable.

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<VarDecl>;

public:
    /**
     * \brief Allocates a new node.
     * \param range the location of the declaration
     * \param name the name of the variable
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range, std::string name) {
        return Ptr(new VarDecl(range, std::move(name)));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(this);
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    VarDecl(SourceRange range, std::string name) : name(std::move(name)), range(range) {
    }

private:
    SourceRange range;
};

/**
 * \brief Represents an identifier.
 */
class Identifier : public Expression {

public:
    std::string name;               //!< The name of the identifier.

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<Identifier>;

public:
    /**
     * \brief Allocates a new node.
     * \param range the location of the identifier
     * \param name the name of the identifier
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceRange range, std::string name) {
        return Ptr(new Identifier(range, std::move(name)));
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(this);
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    Identifier(SourceRange range, std::string name) : name(std::move(name)), range(range) {
    }

private:
    SourceRange range;
};


/**
 * \brief Represents a variable.
 */
class VarRef : public Expression {

public:
    qil::Variable *ref;

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<VarRef>;

public:
    /**
     * \brief Allocates a new node.
     * \param range the location of the declaration
     * \param ref the variable
     * \return a unique pointer to the allocated node
     */
    static VarRef *create(SourceRange range, qil::Variable *ref) {
        return new VarRef(range, ref);
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(this);
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    VarRef(SourceRange range, qil::Variable *ref) : ref(ref), range(range) {
    }

private:
    SourceRange range;
};

/**
 * \brief Represents a string.
 */
class StrRef : public Expression {

public:
    qil::StringLiteral *ref;

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<VarRef>;

public:
    /**
     * \brief Allocates a new node.
     * \param range the location of the declaration
     * \param ref the string
     * \return a unique pointer to the allocated node
     */
    static StrRef *create(SourceRange range, qil::StringLiteral *ref) {
        return new StrRef(range, ref);
    }

    void accept(ExpressionVisitor &v) override {
        v.visit(this);
    }

    SourceRange getRange() const override {
        return range;
    }

private:
    StrRef(SourceRange range, qil::StringLiteral *ref) : ref(ref), range(range) {
    }

private:
    SourceRange range;
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_EXPRESSION_H_
