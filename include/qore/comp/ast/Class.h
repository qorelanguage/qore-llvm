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
/// \brief AST nodes related to classes.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_CLASS_H_
#define INCLUDE_QORE_COMP_AST_CLASS_H_

#include "qore/comp/ast/Namespace.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Base class for all nodes representing class members.
 */
class ClassMember : public Node {

public:
    using Ptr = std::unique_ptr<ClassMember>;               //!< Pointer type.

    /**
     * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Node.
     * \param visitor the visitor to call
     */
    virtual void accept(DeclarationVisitor &visitor) = 0;
};

/**
 * \brief Represents a class.
 */
class Class : public NamespaceMember {

public:
    using Superclass = std::pair<Modifiers, Name>;          //!< The element type of the `inherits` field.

public:
    SourceLocation start;                                   //!< The location of the `class` keyword.
    SourceLocation end;                                     //!< The location of the closing brace or semicolon.
    Modifiers modifiers;                                    //!< The modifiers.
    Name name;                                              //!< The class name.
    std::vector<Superclass> inherits;                       //!< The superclasses.
    std::vector<ClassMember::Ptr> members;                  //!< The members of the class.

public:
    using Ptr = std::unique_ptr<Class>;                     //!< Pointer type.

    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new Class());
    }

    void accept(DeclarationVisitor &visitor) override {
        visitor.visit(*this);
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    Class() {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_CLASS_H_
