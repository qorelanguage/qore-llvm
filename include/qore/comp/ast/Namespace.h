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
/// \brief AST nodes related to namespaces.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_NAMESPACE_H_
#define INCLUDE_QORE_COMP_AST_NAMESPACE_H_

#include <vector>
#include "qore/comp/ast/Const.h"
#include "qore/comp/ast/Routine.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Represents a namespace.
 */
class Namespace : public Declaration {

public:
    SourceLocation start;                                   //!< The location of the `namespace` keyword.
    SourceLocation end;                                     //!< The location of the closing brace or semicolon.
    Modifiers modifiers;                                    //!< The modifiers.
    Name name;                                              //!< The name of the namespace.
    std::vector<Declaration::Ptr> members;                  //!< The members of the namespace.

public:
    using Ptr = std::unique_ptr<Namespace>;                 //!< Pointer type.

    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new Namespace());
    }

    Kind getKind() const override {
        return Kind::Namespace;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    Namespace() : name(Name::invalid()) {
    }
};

/**
 * \brief Represents a function.
 */
class Function : public Declaration {

public:
    Name name;                                              //!< The name of the function.
    Routine::Ptr routine;                                   //!< The function.

public:
    using Ptr = std::unique_ptr<Function>;                  //!< Pointer type.

    /**
     * \brief Allocates a new node.
     * \param name the name of the function
     * \param routine the routine
     * \return a unique pointer to the allocated node
     */
    static Ptr create(Name name, Routine::Ptr routine) {
        return Ptr(new Function(std::move(name), std::move(routine)));
    }

    Kind getKind() const override {
        return Kind::Function;
    }

    SourceLocation getStart() const override {
        return routine->getStart();
    }

    SourceLocation getEnd() const override {
        return routine->getEnd();
    }

private:
    explicit Function(Name name, Routine::Ptr routine) : name(std::move(name)), routine(std::move(routine)) {
    }
};

/**
 * \brief Represents a global variable declaration.
 */
class GlobalVariable : public Declaration {

public:
    SourceLocation start;                                   //!< The location of the 'our' keyword.
    Modifiers modifiers;                                    //!< The modifiers.
    Type type;                                              //!< The type of the variable.
    Name name;                                              //!< The name of the variable.
    SourceLocation end;                                     //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<GlobalVariable>;            //!< Pointer type.

    /**
     * \brief Allocates a new node.
     * \param start the location of the 'our' keyword
     * \param modifiers the modifiers
     * \param type the type of the variable
     * \param name the name of the variable
     * \param end the location of the semicolon
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceLocation start, Modifiers modifiers, Type type, Name name, SourceLocation end) {
        return Ptr(new GlobalVariable(start, modifiers, std::move(type), std::move(name), end));
    }

    Kind getKind() const override {
        return Kind::GlobalVariable;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    GlobalVariable(SourceLocation start, Modifiers modifiers, Type type, Name name, SourceLocation end)
            : start(start), modifiers(modifiers), type(std::move(type)), name(std::move(name)), end(end) {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_NAMESPACE_H_
