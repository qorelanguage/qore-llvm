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
/// \brief Defines the ClosureExpression AST node.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_CLOSURE_H_
#define INCLUDE_QORE_COMP_AST_CLOSURE_H_

#include "qore/comp/ast/Routine.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Represents a closure - a routine that is also an expression.
 */
class ClosureExpression : public Expression {

public:
    Routine::Ptr routine;                                   //!< The closure routine.

public:
    using Ptr = std::unique_ptr<ClosureExpression>;         //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param routine the closure routine
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Routine::Ptr routine) {
        return Ptr(new ClosureExpression(std::move(routine)));
    }

    Kind getKind() const override {
        return Kind::Closure;
    }

    SourceLocation getStart() const override {
        return routine->getStart();
    }

    SourceLocation getEnd() const override {
        return routine->getEnd();
    }

private:
    explicit ClosureExpression(Routine::Ptr routine) : routine(std::move(routine)) {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_CLOSURE_H_
