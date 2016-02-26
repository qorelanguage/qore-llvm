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
/// \brief AST nodes related to the whole Qore script.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_SCRIPT_H_
#define INCLUDE_QORE_COMP_AST_SCRIPT_H_

#include "qore/comp/ast/Namespace.h"
#include "qore/comp/ast/Statement.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Represents the whole script, which is the root namespace with possible top level statements.
 */
class Script : public Node {

public:
    SourceLocation start;                                   //!< The starting location.
    SourceLocation end;                                     //!< The ending location.
    std::vector<NamespaceMember::Ptr> members;              //!< The members of the root namespace.
    std::vector<Statement::Ptr> statements;                 //!< The top level statements.

public:
    using Ptr = std::unique_ptr<Script>;                    //!< Pointer type.

    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new Script());
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    Script() {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_NAMESPACE_H_
