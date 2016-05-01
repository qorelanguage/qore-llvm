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
#include "qore/core/FunctionGroup.h"
#include "qore/core/GlobalVariable.h"

namespace qore {

/**
 * \brief Runtime representation of a namespace.
 *
 * A namespace is a collection of classes, global variables, functions, constants and other namespaces.
 */
class Namespace {

public:
    using Ptr = std::unique_ptr<Namespace>;         //!< Pointer type.

public:
    /**
     * \brief Creates a namespace with given name.
     * \param name the name of the namespace
     */
    explicit Namespace(std::string name) : name(std::move(name)) {
    }

    /**
     * \brief Returns the name of the namespace.
     * \return the name of the namespace
     */
    const std::string &getName() const {
        return name;
    }

    /**
     * \brief Creates a new namespace in this namespace.
     * \param name the name of the namespace
     * \return newly created namespace
     */
    Namespace &addNamespace(std::string name) {
        //assumes that name is unique
        Ptr ptr = Ptr(new Namespace(std::move(name)));
        Namespace &ns = *ptr;
        namespaces.push_back(std::move(ptr));
        return ns;
    }

    /**
     * \brief Creates a new global variable in this namespace.
     *
     * Note that global variables are created uninitialized (without value), see \ref GlobalVariable.
     * \param name the name of the global variable
     * \param type the type of the global variable
     * \return newly created global variable
     */
    GlobalVariable &addGlobalVariable(std::string name, const Type &type) {
        GlobalVariable::Ptr ptr = GlobalVariable::Ptr(new GlobalVariable(std::move(name), type));
        GlobalVariable &gv = *ptr;
        globalVariables.push_back(std::move(ptr));
        return gv;
    }

    /**
     * \brief Creates a new function group in this namespace.
     * \param name the name of the function group
     * \return newly created function group
     */
    FunctionGroup &addFunctionGroup(std::string name) {
        FunctionGroup::Ptr ptr = FunctionGroup::Ptr(new FunctionGroup(std::move(name)));
        FunctionGroup &fg = *ptr;
        functionGroups.push_back(std::move(ptr));
        return fg;
    }

private:
    Namespace(const Namespace &) = delete;
    Namespace(Namespace &&) = delete;
    Namespace &operator=(const Namespace &) = delete;
    Namespace &operator=(Namespace &&) = delete;

private:
    std::string name;
    std::vector<std::unique_ptr<Namespace>> namespaces;
    std::vector<std::unique_ptr<GlobalVariable>> globalVariables;
    std::vector<std::unique_ptr<FunctionGroup>> functionGroups;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_NAMESPACE_H_
