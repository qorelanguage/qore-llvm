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
#ifndef INCLUDE_QORE_COMP_AST_NODE_H_
#define INCLUDE_QORE_COMP_AST_NODE_H_

#include <memory>
#include "qore/comp/SourceLocation.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Base class for all nodes in the Abstract Syntax Tree.
 */
class Node {

public:
    using Ptr = std::unique_ptr<Node>;                      //!< Pointer type.

public:
    virtual ~Node() = default;

    /**
     * \brief Returns the start location of this node in the source code.
     * \return the start location
     */
    virtual SourceLocation getStart() const = 0;

    /**
     * \brief Returns the end location of this node in the source code.
     * \return the end location
     */
    virtual SourceLocation getEnd() const = 0;

protected:
    Node() = default;

private:
    Node(const Node &) = delete;
    Node(Node &&) = delete;
    Node &operator=(const Node &) = delete;
    Node &operator=(Node &&) = delete;
};

/**
 * \brief Base class for all nodes representing declarations.
 */
class Declaration : public Node {

public:
    using Ptr = std::unique_ptr<Declaration>;               //!< Pointer type.

    /**
     * \brief Identifies the type of the declaration.
     */
    enum class Kind {
        Namespace,          //!< Identifies instances of \ref Namespace.
        Class,              //!< Identifies instances of \ref Class.
        Function,           //!< Identifies instances of \ref Function.
        Constant,           //!< Identifies instances of \ref NamespaceConstant.
        GlobalVariable,     //!< Identifies instances of \ref GlobalVariable.
    };

    /**
     * \brief Returns the type of the declaration.
     * \return the type of the declaration
     */
    virtual Kind getKind() const = 0;
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_NODE_H_
