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
/// \brief Abstract base for all AST nodes.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_NODE_H_
#define INCLUDE_QORE_AST_NODE_H_

#include "qore/ast/Visitor.h"
#include "qore/context/SourceRange.h"

namespace qore {

/**
 * \brief Abstract Syntax Tree namespace.
 */
namespace ast {

/**
 * \brief Base class for all nodes in the Abstract Syntax Tree.
 */
class Node {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<Node>;

public:
    virtual ~Node() = default;

    /**
     * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Node.
     * \param visitor the visitor to call
     */
    virtual void accept(Visitor &visitor) const = 0;

    /**
     * \brief Returns the range of locations in the source code of this node.
     * \return the range of locations
     */
    virtual SourceRange getRange() const = 0;

protected:
    Node() = default;

private:
    Node(const Node &) = delete;
    Node(Node &&) = delete;
    Node &operator=(const Node &) = delete;
    Node &operator=(Node &&) = delete;
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_ASTNODE_H_
