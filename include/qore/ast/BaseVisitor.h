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
/// \brief Specialized visitor base.
///
//------------------------------------------------------------------------------

#ifndef INCLUDE_QORE_AST_BASEVISITOR_H_
#define INCLUDE_QORE_AST_BASEVISITOR_H_

#include "qore/ast/Visitor.h"
#include "qore/common/Util.h"

namespace qore {
namespace ast {

/**
 * \brief Base class for specialized visitors that implement only some of the methods.
 */
class BaseVisitor : public Visitor {

public:
    /**
     * \brief Constructor.
     * \param name the name of the visitor for debugging purposes
     */
    BaseVisitor(const char *name = "<unknown>") : name(name) {
    }

    void visit(const IntegerLiteral *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const StringLiteral *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const BinaryExpression *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const UnaryExpression *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const Assignment *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const VarDecl *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const Identifier *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const EmptyStatement *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const PrintStatement *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const ExpressionStatement *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

    void visit(const Program *node) override {
        QORE_UNREACHABLE(name << " does not support this type of node");
    }

private:
    const char *name;
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_BASEVISITOR_H_
