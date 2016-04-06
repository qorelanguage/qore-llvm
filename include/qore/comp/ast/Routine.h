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
/// \brief Defines the Routine AST node.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_ROUTINE_H_
#define INCLUDE_QORE_COMP_AST_ROUTINE_H_

#include <tuple>
#include <utility>
#include <vector>
#include "qore/comp/ast/Modifiers.h"
#include "qore/comp/ast/Statement.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Represents a routine - function, method, constructor, etc.
 */
class Routine : public Node {

public:
    /**
     * \brief The type of one element of the parameter list: a triplet (type, name, default value (may be nullptr)).
     */
    using Param = std::tuple<Type, Token, Expression::Ptr>;

    /**
     * \brief The type of one element of the base constructor invocation list.
     */
    using BaseCtorInvocation = std::pair<Name, ArgList::Ptr>;

public:
    Modifiers modifiers;                                    //!< The modifiers.
    Type type;                                              //!< The return type.
    std::vector<Param> params;                              //!< The parameters.
    std::vector<BaseCtorInvocation> baseCtors;              //!< The base constructor invocation list.
    CompoundStatement::Ptr body;                            //!< The body of the routine, nullptr for abstract methods.
    SourceLocation semicolon;                               //!< The location of semicolon (if body is nullptr)

public:
    using Ptr = std::unique_ptr<Routine>;                   //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new Routine());
    }

    SourceLocation getStart() const override {
        return type.getStart();
    }

    SourceLocation getEnd() const override {
        return body ? body->getEnd() : semicolon;
    }

private:
    Routine() : type(Type::createInvalid()) {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_ROUTINE_H_
