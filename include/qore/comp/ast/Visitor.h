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

/**
 * \brief Interface for expression visitors.
 */
class ExpressionVisitor {

public:
    virtual ~ExpressionVisitor() {}

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

protected:
    DeclarationVisitor() = default;

private:
    DeclarationVisitor(const DeclarationVisitor &) = delete;
    DeclarationVisitor(DeclarationVisitor &&) = delete;
    DeclarationVisitor &operator=(const DeclarationVisitor &) = delete;
    DeclarationVisitor &operator=(DeclarationVisitor &&) = delete;
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_VISITOR_H_
