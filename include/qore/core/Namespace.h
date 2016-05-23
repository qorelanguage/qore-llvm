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
/// \brief Runtime representation of a namespace.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_NAMESPACE_H_
#define INCLUDE_QORE_CORE_NAMESPACE_H_

#include <string>
#include <vector>
#include "qore/core/Class.h"
#include "qore/core/FunctionGroup.h"
#include "qore/core/GlobalVariable.h"
#include "qore/core/util/Iterators.h"

namespace qore {

/**
 * \brief Runtime representation of a namespace.
 *
 * A namespace is a collection of classes, global variables, functions, constants and other namespaces.
 */
class Namespace {

public:
    using Ptr = std::unique_ptr<Namespace>;                                             //!< Pointer type.
    using NamespaceIterator = util::VectorOfPtrIteratorAdapter<Namespace>;              //!< Namespace iterator.
    using ClassIterator = util::VectorOfPtrIteratorAdapter<Class>;                      //!< Class iterator.
    using GlobalVariableIterator = util::VectorOfPtrIteratorAdapter<GlobalVariable>;    //!< Globals iterator.
    using FunctionGroupIterator = util::VectorOfPtrIteratorAdapter<FunctionGroup>;      //!< Function groups iterator.

public:
    /**
     * \brief Creates a namespace with given name.
     * \param fullName the full name of the namespace
     * \param location the location of the (first) declaration of the namespace
     */
    Namespace(std::string fullName, SourceLocation location) : fullName(std::move(fullName)), location(location) {
        assert(this->fullName == "" || this->fullName.find(':') != std::string::npos);
    }

    /**
     * \brief Returns the full name of the namespace.
     * \return the full name of the namespace
     */
    const std::string &getFullName() const {
        return fullName;
    }

    /**
     * \brief Returns the location of the declaration.
     * \return the location of the declaration
     */
    const SourceLocation &getLocation() const {
        return location;
    }

    /**
     * \brief Creates a new namespace in this namespace.
     * \param name the name (relative to this namespace) of the namespace
     * \param location the location of the (first) declaration
     * \return newly created namespace
     */
    Namespace &addNamespace(const std::string &name, SourceLocation location) {
        assert(name.find(':') == std::string::npos);
        Ptr ptr = Ptr(new Namespace(fullName + "::" + name, location));
        Namespace &ns = *ptr;
        namespaces.push_back(std::move(ptr));
        return ns;
    }

    /**
     * \brief Creates a new class in this namespace.
     * \param name the name (relative to this namespace) of the class
     * \param location the location of the declaration
     * \return newly created class
     */
    Class &addClass(const std::string &name, SourceLocation location) {
        assert(name.find(':') == std::string::npos);
        Class::Ptr ptr = Class::Ptr(new Class(fullName + "::" + name, location));
        Class &c = *ptr;
        classes.push_back(std::move(ptr));
        return c;
    }

    /**
     * \brief Creates a new global variable in this namespace.
     *
     * Note that global variables are created uninitialized (without value), see \ref GlobalVariable.
     * \param name the name (relative to this namespace) of the global variable
     * \param type the type of the global variable
     * \param location the location of the declaration in the source
     * \return newly created global variable
     */
    GlobalVariable &addGlobalVariable(const std::string &name, const Type &type, SourceLocation location) {
        assert(name.find(':') == std::string::npos);
        GlobalVariable::Ptr ptr = GlobalVariable::Ptr(new GlobalVariable(fullName + "::" + name, type, location));
        GlobalVariable &gv = *ptr;
        globalVariables.push_back(std::move(ptr));
        return gv;
    }

    /**
     * \brief Creates a new function group in this namespace.
     * \param name the name (relative to this namespace) of the function group
     * \return newly created function group
     */
    FunctionGroup &addFunctionGroup(const std::string &name) {
        assert(name.find(':') == std::string::npos);
        FunctionGroup::Ptr ptr = FunctionGroup::Ptr(new FunctionGroup(fullName + "::" + name));
        FunctionGroup &fg = *ptr;
        functionGroups.push_back(std::move(ptr));
        return fg;
    }

    /**
     * \brief Returns a range for iterating namespaces.
     * \return a range for iterating namespaces
     */
    util::IteratorRange<NamespaceIterator> getNamespaces() const {
        return util::IteratorRange<NamespaceIterator>(namespaces);
    }

    /**
     * \brief Returns a range for iterating classes.
     * \return a range for iterating classes
     */
    util::IteratorRange<ClassIterator> getClasses() const {
        return util::IteratorRange<ClassIterator>(classes);
    }

    /**
     * \brief Returns a range for iterating global variables.
     * \return a range for iterating global variables
     */
    util::IteratorRange<GlobalVariableIterator> getGlobalVariables() const {
        return util::IteratorRange<GlobalVariableIterator>(globalVariables);
    }

    /**
     * \brief Returns a range for iterating function groups.
     * \return a range for iterating function groups
     */
    util::IteratorRange<FunctionGroupIterator> getFunctionGroups() const {
        return util::IteratorRange<FunctionGroupIterator>(functionGroups);
    }

private:
    Namespace(const Namespace &) = delete;
    Namespace(Namespace &&) = delete;
    Namespace &operator=(const Namespace &) = delete;
    Namespace &operator=(Namespace &&) = delete;

private:
    std::string fullName;
    SourceLocation location;
    std::vector<Namespace::Ptr> namespaces;
    std::vector<Class::Ptr> classes;
    std::vector<GlobalVariable::Ptr> globalVariables;
    std::vector<FunctionGroup::Ptr> functionGroups;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_NAMESPACE_H_
