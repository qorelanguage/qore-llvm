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
/// \brief Program AST node.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_PROGRAM_H_
#define INCLUDE_QORE_AST_PROGRAM_H_

#include "qore/ast/Statement.h"

namespace qore {
namespace ast {

/**
 * \brief Represents the whole script.
 */
class Program : public Node {

public:
    /**
     * \brief Pointer type.
     */
    using Ptr = std::unique_ptr<Program>;

public:
    Statements body;        //!< The statements of the script.

public:
    /**
     * \brief Allocates a new node.
     * \param body the statements of the script
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Statements body) {
        return Ptr(new Program(std::move(body)));
    }

    void *accept(Visitor &v) const override {
        return v.visit(this);
    }

private:
    Program(Statements body) : body(std::move(body)) {
    }
};

} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_PROGRAM_H_
