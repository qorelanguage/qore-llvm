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
/// \brief Defines the Core class which holds the state shared by the analyzer classes.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_CORE_H_
#define INCLUDE_QORE_COMP_SEM_CORE_H_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "qore/comp/Context.h"
#include "qore/comp/ast/Statement.h"
#include "qore/comp/sem/Scope.h"
#include "qore/comp/sem/expr/Expression.h"
#include "qore/comp/sem/stmt/Statement.h"

namespace qore {
namespace comp {
namespace sem {

class Builder;
class NamespaceScope;
class ClassScope;
class GlobalVariableInfo;
class FunctionGroupInfo;
class FunctionScope;

/**
 * \brief Provides state shared by other analyzer classes and some useful functions.
 */
class Core {

public:
    /**
     * \brief Constructor.
     * \param ctx the compiler context
     */
    explicit Core(Context &ctx);

    /**
     * \brief Returns the compiler context.
     * \return the compiler context
     */
    Context &getContext() const {
        return ctx;
    }

    /**
     * \brief Reports a diagnostic message.
     *
     * Shortcut for
     * \code
     *     getContext().report(diagId, location);
     * \endcode
     * \param diagId the identifier of the diagnostic message
     * \param location the location in the source
     * \return a builder for providing additional info
     */
    DiagBuilder report(DiagId diagId, SourceLocation location) const {
        return ctx.report(diagId, location);
    }

    /**
     * \brief Returns the string identified by given reference.
     *
     * Shortcut for
     * \code
     *     getContext().getString(ref);
     * \endcode
     * \param ref a valid reference to the string table
     * \return the string value
     */
    const std::string &getString(String::Ref ref) const {
        return ctx.getString(ref);
    }

    ///\name Queue management methods
    ///\{
    /**
     * \brief Puts a class into the 'class queue'.
     * \param c the class to enqueue
     */
    void addToQueue(ClassScope &c) {
        classQueue.push_back(&c);
    }

    /**
     * \brief Puts a class into the 'global variable queue'.
     * \param gv the global variable to enqueue
     */
    void addToQueue(GlobalVariableInfo &gv) {
        globalVariableQueue.push_back(&gv);
    }

    /**
     * \brief Puts a function group into the 'function group queue'.
     * \param fg the function group to enqueue
     */
    void addToQueue(FunctionGroupInfo &fg) {
        functionGroupQueue.push_back(&fg);
    }

    /**
     * \brief Puts a function into the 'function queue'.
     * \param f the function to enqueue
     */
    void addToQueue(FunctionScope &f) {
        functionQueue.emplace_back(&f);
    }

    /**
     * \brief Adds a global variable or constant initializer to an array of initializers.
     * \param stmt the global variable or constant initializer
     */
    void addInitializer(Statement::Ptr stmt) {
        initializers.push_back(std::move(stmt));
    }

    /**
     * \brief Returns the 'class queue', clearing the queue in this instance (move semantics).
     * \return the 'class queue'
     */
    std::vector<ClassScope *> takeClassQueue() {
        return std::move(classQueue);
    }

    /**
     * \brief Returns the 'global variable queue', clearing the queue in this instance (move semantics).
     * \return the 'global variable queue'
     */
    std::vector<GlobalVariableInfo *> takeGlobalVariableQueue() {
        return std::move(globalVariableQueue);
    }

    /**
     * \brief Returns the 'function group queue', clearing the queue in this instance (move semantics).
     * \return the 'function group queue'
     */
    std::vector<FunctionGroupInfo *> takeFunctionGroupQueue() {
        return std::move(functionGroupQueue);
    }

    /**
     * \brief Returns the 'function queue', clearing the queue in this instance (move semantics).
     * \return the 'function queue'
     */
    std::vector<FunctionScope *> takeFunctionQueue() {
        return std::move(functionQueue);
    }

    /**
     * \brief Returns the array of initializer, clearing it in this instance (move semantics).
     * \return the array of initializers
     */
    std::vector<Statement::Ptr> takeInitializers() {
        return std::move(initializers);
    }
    ///\}

    /**
     * \brief Creates a \ref String instance for given string literal.
     *
     * Ensures that string literals with the same sequence of characters share the same instance of \ref String.
     * \param value the value of the string literal
     * \return an instance of \ref String
     */
    qore::String &createStringLiteral(const std::string &value);

    /**
     * \brief Returns a constant expression representing the default value for given type.
     * \param type the type
     * \return a constant expression representing the default value
     * \todo define what should be done for types with no default (probably report an error and return NothingLiteral)
     */
    Expression::Ptr defaultFor(const Type &type);

    /**
     * \brief Returns the built-in type represented by given identifier.
     * \param name the identifier and location
     * \param asterisk true if the asterisk version of the type should be returned
     * \return the resolved type or nullptr if 'name' is not the name of any built-in type
     */
    const Type *getBuiltinType(const ast::Name::Id &name, bool asterisk) const;

private:
    Core(const Core &) = delete;
    Core(Core &&) = delete;
    Core &operator=(const Core &) = delete;
    Core &operator=(Core &&) = delete;

private:
    Context &ctx;
    std::vector<ClassScope *> classQueue;
    std::vector<GlobalVariableInfo *> globalVariableQueue;
    std::vector<FunctionGroupInfo *> functionGroupQueue;
    std::vector<FunctionScope *> functionQueue;
    std::vector<Statement::Ptr> initializers;
    std::unordered_map<std::string, qore::String *> strings;
    std::map<String::Ref, std::pair<const Type *, const Type *>> builtinTypes;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_CORE_H_
