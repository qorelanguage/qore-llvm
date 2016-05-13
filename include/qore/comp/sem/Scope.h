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
/// \brief Defines the Scope interface.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_SCOPE_H_
#define INCLUDE_QORE_COMP_SEM_SCOPE_H_

#include "qore/core/Type.h"
#include "qore/comp/ast/Type.h"
#include "qore/comp/sem/GlobalVariableInfo.h"
#include "qore/comp/sem/LocalVariableInfo.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Represents the result of symbol resolution.
 */
class Symbol {

public:
    /**
     * \brief Identifies the type of the resolved symbol.
     */
    enum class Kind {
        Global,     //!< Identifies a resolved global variable.
        Local,      //!< Identifies a resolved local variable.
    };

public:
    /**
     * \brief Creates an instance representing a global variable.
     * \param gv the resolved global variable
     */
    explicit Symbol(const GlobalVariableInfo &gv) : kind(Kind::Global), ptr(&gv) {
    }

    /**
     * \brief Creates an instance representing a local variable.
     * \param lv the resolved local variable
     */
    explicit Symbol(const LocalVariableInfo &lv) : kind(Kind::Local), ptr(&lv) {
    }

    /**
     * \brief Returns the type of the resolved symbol.
     * \return the type of the resolved symbol
     */
    Kind getKind() const {
        return kind;
    }

    /**
     * \brief Returns the resolved symbol as a global variable.
     *
     * This method can be used only if \ref getKind() returns \ref Kind::Global.
     * \return the resolved symbol as a global variable
     */
    const GlobalVariableInfo &asGlobal() const {
        assert(kind == Kind::Global);
        return *static_cast<const GlobalVariableInfo *>(ptr);
    }

    /**
     * \brief Returns the resolved symbol as a local variable.
     *
     * This method can be used only if \ref getKind() returns \ref Kind::Local.
     * \return the resolved symbol as a local variable
     */
    const LocalVariableInfo &asLocal() const {
        assert(kind == Kind::Local);
        return *static_cast<const LocalVariableInfo *>(ptr);
    }

private:
    Kind kind;
    const void *ptr;
};

/**
 * \brief Interface for entities providing scope functions such as symbol resolution.
 */
class Scope {

public:
    virtual ~Scope() = default;

    /**
     * \brief Resolves a type.
     *
     * If the type cannot be resolved, reports an error and returns the \ref Type::Error type.
     * \param node the AST node representing the type
     * \return resolved type
     */
    virtual const Type &resolveType(const ast::Type &node) const = 0;

    /**
     * \brief Resolves a symbol.
     * \param name the AST node representing the name to resolve
     * \return resolved symbol
     * \todo define what should happen if the name cannot be resolved
     */
    virtual Symbol resolveSymbol(const ast::Name &name) const = 0;

    /**
     * \brief Creates a local variable.
     *
     * The class that implements this method is the owner of the LocalVariableInfo instance.
     * \param name the name of the local variable
     * \param location the location of the local variable
     * \param type the type of the local variable
     * \return the new local variable
     * \todo define what should happen if there is no function scope, e.g. `our string s = string x;` or
     * `class C { constructor() : base(5 + int i) {} }` etc.
     */
    virtual LocalVariableInfo &createLocalVariable(String::Ref name, SourceLocation location, const Type &type) = 0;

    /**
     * \brief Declares a local variable.
     *
     * The class that implements this method (BlockScope) calls \ref createLocalVariable to delegate the ownership
     * of the local variable to the nearest FunctionScope.
     * \param name the name of the local variable
     * \param location the location of the local variable
     * \param type the type of the local variable
     * \return the local variable
     */
    virtual LocalVariableInfo &declareLocalVariable(String::Ref name, SourceLocation location, const Type &type) = 0;

    /**
     * \brief Returns the type for the return value of the nearest function scope.
     * \return the type of the return value of the nearest function scope
     */
    virtual const Type &getReturnType() const = 0;

protected:
    Scope() = default;

private:
    Scope(const Scope &) = delete;
    Scope(Scope &&) = delete;
    Scope &operator=(const Scope &) = delete;
    Scope &operator=(Scope &&) = delete;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_SCOPE_H_
