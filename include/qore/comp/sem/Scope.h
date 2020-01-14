//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 - 2020 Qore Technologies, s.r.o.
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
#include "qore/core/Function.h"
#include "qore/comp/ast/Routine.h"
#include "qore/comp/sem/GlobalVariableInfo.h"
#include "qore/comp/sem/LocalVariableInfo.h"
#include "qore/comp/sem/stmt/Statement.h"

namespace qore {

class FunctionGroup;

namespace comp {
namespace sem {

class FunctionGroupInfo;
class GlobalVariableInfo;

/**
 * \brief Represents the result of symbol resolution.
 */
class Symbol {

public:
    /**
     * \brief Identifies the type of the resolved symbol.
     */
    enum class Kind {
        FunctionGroup,  //!< Identifies a resolved function group.
        Global,         //!< Identifies a resolved global variable.
        Local,          //!< Identifies a resolved local variable.
    };

public:
    /**
     * \brief Creates an instance representing a function group.
     * \param fg the resolved function group
     */
    explicit Symbol(const FunctionGroupInfo &fg) : kind(Kind::FunctionGroup), ptr(&fg) {
    }

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
     * \brief Returns the resolved symbol as a function group.
     *
     * This method can be used only if \ref getKind() returns \ref Kind::FunctionGroup.
     * \return the resolved symbol as a function group
     */
    const FunctionGroupInfo &asFunctionGroup() const {
        assert(kind == Kind::FunctionGroup);
        return *static_cast<const FunctionGroupInfo *>(ptr);
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

class Core;

/**
 * \brief Describes a function during semantic analysis and implements its scope.
 */
class FunctionScope : public Scope {

public:
    using Ptr = std::unique_ptr<FunctionScope>;     //!< Pointer type.

public:
    /**
     * \brief Constructor.
     * \param rt the runtime object representing the function
     * \param core the shared state of the analyzer
     * \param parent the parent namespace scope
     * \param node the AST node
     */
    FunctionScope(Function &rt, Core &core, Scope &parent, ast::Routine &node);

    const Type &resolveType(const ast::Type &node) const override;

    LocalVariableInfo &createLocalVariable(String::Ref name, SourceLocation location, const Type &type) override;

    Symbol resolveSymbol(const ast::Name &name) const override;

    LocalVariableInfo &declareLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        QORE_UNREACHABLE("");
    }

    const Type &getReturnType() const override {
        return rt.getType().getReturnType();
    }

    /**
     * \brief Performs the analysis of the function's body, i.e. translation to the \ref qore::code representation.
     * \param initializers optional initializer statements to prepend before the function's body
     */
    void analyze(std::vector<Statement::Ptr> *initializers = nullptr);

private:
    Function &rt;
    Core &core;
    Scope &parent;
    ast::Routine &node;
    std::vector<LocalVariableInfo::Ptr> locals;
    std::map<String::Ref, LocalVariableInfo *> args;
};

/**
 * \brief Describes a group of function overloads during semantic analysis.
 */
class FunctionGroupInfo {

public:
    using Ptr = std::unique_ptr<FunctionGroupInfo>;     //!< Pointer type.

public:
    /**
     * \brief Constructor.
     * \param rt the runtime object representing the group of function overloads
     * \param core the shared state of the analyzer
     * \param parent the parent namespace scope
     * \param location the location of the (first) declaration
     */
    FunctionGroupInfo(FunctionGroup &rt, Core &core, Scope &parent, SourceLocation location)
            : rt(rt), core(core), parent(parent), location(location) {
    }

    /**
     * \brief Returns the location of the first declaration.
     * \return the location of the first declaration
     */
    SourceLocation getLocation() const {
        return location;
    }

    /**
     * \brief Returns the runtime object representing the function group.
     * \return the runtime object representing the function group
     */
    FunctionGroup &getRt() const {
        return rt;
    }

    /**
     * \brief Adds an overload to the group.
     * \param node the AST node of the function
     * \param location the location of the overload
     */
    void addOverload(ast::Routine &node, SourceLocation location);

    /**
     * \brief Resolves the types of the overloads and check for uniqueness.
     */
    void pass2();

private:
    void checkOverload(FunctionType &type);

private:
    FunctionGroup &rt;
    Core &core;
    Scope &parent;
    SourceLocation location;
    std::vector<std::pair<ast::Routine *, SourceLocation>> queue;
    std::vector<FunctionScope::Ptr> functions;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_SCOPE_H_
