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
    Class() : name(Name::invalid()) {
    }
};

/**
 * \brief Represents a method (including special methods such as constructors).
 */
class Method : public ClassMember {

public:
    Name name;                                              //!< The name of the method.
    Routine::Ptr routine;                                   //!< The method.

public:
    using Ptr = std::unique_ptr<Method>;                    //!< Pointer type.

    /**
     * \brief Allocates a new node.
     * \param name the name of the method
     * \param routine the routine
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Name name, Routine::Ptr routine) {
        return Ptr(new Method(std::move(name), std::move(routine)));
    }

    void accept(DeclarationVisitor &visitor) override {
        visitor.visit(*this);
    }

    SourceLocation getStart() const override {
        return routine->getStart();
    }

    SourceLocation getEnd() const override {
        return routine->getEnd();
    }

private:
    explicit Method(Name name, Routine::Ptr routine) : name(std::move(name)), routine(std::move(routine)) {
    }
};

/**
 * \brief Represents a constant definition in a class.
 */
class ClassConstant : public ClassMember {

public:
    Constant::Ptr constant;                                 //!< The constant.

public:
    using Ptr = std::unique_ptr<ClassConstant>;             //!< Pointer type.

    /**
     * \brief Allocates a new node.
     * \param constant the constant
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Constant::Ptr constant) {
        return Ptr(new ClassConstant(std::move(constant)));
    }

    void accept(DeclarationVisitor &visitor) override {
        visitor.visit(*this);
    }

    SourceLocation getStart() const override {
        return constant->getStart();
    }

    SourceLocation getEnd() const override {
        return constant->getEnd();
    }

private:
    explicit ClassConstant(Constant::Ptr constant) : constant(std::move(constant)) {
    }
};

/**
 * \brief Represents a group of members in a class.
 */
class MemberGroup : public ClassMember {

public:
    SourceLocation start;                                   //!< The location of the opening brace.
    SourceLocation end;                                     //!< The location of the closing brace.
    Modifiers modifiers;                                    //!< The modifiers of the whole group.
    std::vector<ClassMember::Ptr> members;                  //!< The members of the group.

public:
    using Ptr = std::unique_ptr<MemberGroup>;               //!< Pointer type.

    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new MemberGroup());
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
    MemberGroup() {
    }
};

/**
 * \brief Represents a field definition in a class.
 *
 * At most one of the exprInit and argListInit can be used.
 */
class Field : public ClassMember {

public:
    Modifiers modifiers;                                    //!< The modifiers.
    Type::Ptr type;                                         //!< The type of the field.
    Token name;                                             //!< The name of the field.
    SourceLocation end;                                     //!< The location of the semicolon.
    Expression::Ptr exprInit;                               //!< The expression initializer.
    ArgList::Ptr argListInit;                               //!< The argList initializer.

public:
    using Ptr = std::unique_ptr<Field>;                     //!< Pointer type.

    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new Field());
    }

    void accept(DeclarationVisitor &visitor) override {
        visitor.visit(*this);
    }

    SourceLocation getStart() const override {
        return type->getStart();
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    Field() {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_CLASS_H_
