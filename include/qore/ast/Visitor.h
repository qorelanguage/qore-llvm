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

/**
 * \brief Interface for implementations of the visitor pattern for the AST.
 */
class Visitor {

public:
    virtual ~Visitor() {}

    /**
     * \brief Called by an IntegerLiteral AST node.
     * \param node the node being visited
     * \return defined by the implementation
     */
    virtual void* visit(const class IntegerLiteral *node) = 0;

    /**
     * \brief Called by a StringLiteral AST node.
     * \param node the node being visited
     * \return defined by the implementation
     */
    virtual void* visit(const class StringLiteral *node) = 0;

    /**
     * \brief Called by a BinaryExpression AST node.
     * \param node the node being visited
     * \return defined by the implementation
     */
    virtual void* visit(const class BinaryExpression *node) = 0;

    /**
     * \brief Called by a EmptyStatement AST node.
     * \param node the node being visited
     * \return defined by the implementation
     */
    virtual void* visit(const class EmptyStatement *node) = 0;

    /**
     * \brief Called by a PrintStatement AST node.
     * \param node the node being visited
     * \return defined by the implementation
     */
    virtual void* visit(const class PrintStatement *node) = 0;

    /**
     * \brief Called by a Program AST node.
     * \param node the node being visited
     * \return defined by the implementation
     */
    virtual void* visit(const class Program *node) = 0;

protected:
    Visitor() = default;

private:
    Visitor(const Visitor &) = delete;
    Visitor(Visitor &&) = delete;
    Visitor &operator=(const Visitor &) = delete;
    Visitor &operator=(Visitor &&) = delete;
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_VISITOR_H_
