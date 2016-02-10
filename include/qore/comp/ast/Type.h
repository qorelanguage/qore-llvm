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
/// \brief Defines type AST nodes.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_TYPE_H_
#define INCLUDE_QORE_COMP_AST_TYPE_H_

#include "qore/comp/Token.h"
#include "qore/comp/ast/Name.h"
#include "qore/comp/ast/Visitor.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Base class for all nodes representing types.
 */
class Type : public Node {

public:
    using Ptr = std::unique_ptr<Type>;                      //!< Pointer type.

    /**
     * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Node.
     * \param visitor the visitor to call
     */
    virtual void accept(TypeVisitor &visitor) = 0;
};

/**
 * \brief Represents simple type expressed by a name.
 */
class NameType : public Type {

public:
    Name name;                                              //!< The name of the type.

public:
    using Ptr = std::unique_ptr<NameType>;                  //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param name the name of the type
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Name name) {
        return Ptr(new NameType(std::move(name)));
    }

    void accept(TypeVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return name.getStart();
    }

    SourceLocation getEnd() const override {
        return name.getEnd();
    }

private:
    explicit NameType(Name name) : name(std::move(name)) {
    }
};

/**
 * \brief Represents type expressed by an asterisk followed by a name.
 */
class AsteriskType : public Type {

public:
    SourceLocation asterisk;                                //!< The location of the asterisk.
    Name name;                                              //!< The name of the type.

public:
    using Ptr = std::unique_ptr<AsteriskType>;              //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param asterisk the location of the asterisk
     * \param name the name of the type
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceLocation asterisk, Name name) {
        return Ptr(new AsteriskType(asterisk, std::move(name)));
    }

    void accept(TypeVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return asterisk;
    }

    SourceLocation getEnd() const override {
        return name.getEnd();
    }

private:
    explicit AsteriskType(SourceLocation asterisk, Name name) : asterisk(asterisk), name(std::move(name)) {
    }
};

/**
 * \brief Represents the implicit 'nothing' type of functions without return type declaration.
 */
class ImplicitType : public Type {

public:
    SourceLocation location;                                //!< The location of the 'sub' declaration

public:
    using Ptr = std::unique_ptr<ImplicitType>;              //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ImplicitType());
    }

    void accept(TypeVisitor &v) override {
        v.visit(*this);
    }

    SourceLocation getStart() const override {
        return location;
    }

    SourceLocation getEnd() const override {
        return location;
    }

private:
    ImplicitType() {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_TYPE_H_
