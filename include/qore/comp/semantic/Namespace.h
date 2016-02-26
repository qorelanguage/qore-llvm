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
/// \brief Representation of namespaces during semantic analysis.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_NAMESPACE_H_
#define INCLUDE_QORE_COMP_SEMANTIC_NAMESPACE_H_

#include <map>
#include <string>
#include "qore/common/Util.h"
#include "qore/comp/semantic/Context.h"
#include "qore/comp/semantic/Constant.h"
#include "qore/comp/semantic/Function.h"
#include "qore/comp/semantic/GlobalVariable.h"

namespace qore {
namespace comp {
namespace semantic {

class Class;

/**
 * \brief Represents a namespace.
 */
class Namespace : public NamedScope {

public:
    using Ptr = std::unique_ptr<Namespace>;                 //!< Pointer type.

public:
    /**
     * \brief Creates the root namespace.
     * \param context the context for semantic analysis
     * \return the root namespace
     */
    static Ptr createRoot(Context &context) {
        return Ptr(new Namespace(context, nullptr, SourceLocation(), "<root>"));
    }

    /**
     * \brief Creates a new namespace.
     * \param parent the parent namespace
     * \param location the location of the (first) declaration
     * \param name the name of the namespace
     * \return the namespace
     */
    static Ptr create(Namespace &parent, SourceLocation location, std::string name) {
        return Ptr(new Namespace(parent.context, &parent, location, std::move(name)));
    }

    /**
     * \brief Return true if this instance represents the root namespace.
     * \return true if this instance represents the root namespace
     */
    bool isRoot() const {
        return parent == nullptr;
    }

    /**
     * \brief Returns the location of the (first) namespace declaration.
     *
     * It is illegal to call this method for root namespace since it has no declaration.
     * \return the location of the namespace declaration.
     */
    const SourceLocation &getLocation() const {
        assert(!isRoot());
        return location;
    }

    /**
     * \brief Returns the name of the namespace.
     * \return the name of the namespace
     */
    const std::string getName() const {
        return name;
    }

    std::string getFullName() const override {
        return parent ? parent->getFullName() + "::" + name : "";
    }

    /**
     * \brief Calls a function for each member namespace.
     * \param callback the callback to call
     */
    void forEachNamespace(std::function<void(Namespace &)> callback) const {
        for (auto &n : namespaces) {
            callback(*n.second);
        }
    }

    /**
     * \brief Calls a function for each member class.
     * \param callback the callback to call
     */
    void forEachClass(std::function<void(Class &)> callback) const {
        for (auto &c : classes) {
            callback(*c.second);
        }
    }

    /**
     * \brief Calls a function for each member constant.
     * \param callback the callback to call
     */
    void forEachConstant(std::function<void(Constant &)> callback) const {
        for (auto &c : constants) {
            callback(*c.second);
        }
    }

    /**
     * \brief Calls a function for each function group.
     * \param callback the callback to call
     */
    void forEachFunctionGroup(std::function<void(const Function::Group &)> callback) const {
        for (auto &f : functionGroups) {
            callback(f.second);
        }
    }

    /**
     * \brief Calls a function for each member global variable.
     * \param callback the callback to call
     * \param recursive if true, the callback is called for global variables declared in nested namespaces as well
     */
    void forEachGlobalVariable(std::function<void(GlobalVariable &)> callback, bool recursive = false) const {
        if (recursive) {
            for (auto &n : namespaces) {
                n.second->forEachGlobalVariable(callback, true);
            }
        }
        for (auto &gv : globalVariables) {
            callback(*gv.second);
        }
    }

    /**
     * \brief Finds a namespace with given name.
     * \param name the name to find
     * \return the namespace or `nullptr` if not found
     */
    Namespace *findNamespace(const std::string &name) const {
        auto it = namespaces.find(name);
        return it == namespaces.end() ? nullptr : it->second.get();
    }

    /**
     * \brief Finds a class with given name.
     * \param name the name to find
     * \return the class or `nullptr` if not found
     */
    Class *findClass(const std::string &name) const {
        auto it = classes.find(name);
        return it == classes.end() ? nullptr : it->second.get();
    }

    /**
     * \brief Adds a namespace to this namespace.
     *
     * If another class with the same name exists, reports an error and does not add the namespace.
     * If another namespace with the same name exists, the behavior is undefined.
     * \param ns the namespace to add
     * \return true if added successfully
     */
    bool addNamespace(Ptr ns);

    /**
     * \brief Adds a class to this namespace.
     *
     * If another class or namespace with the same name exists, reports an error and does not add the class.
     * \param c the class to add
     */
    void addClass(std::unique_ptr<Class> c);

    /**
     * \brief Adds a constant to this namespace.
     *
     * If another constant, function or global variable with the same name exists, reports an error and
     * does not add the constant to the namespace.
     * \param c the constant to add
     */
    void addConstant(Constant::Ptr c);

    /**
     * \brief Adds a function to this namespace.
     *
     * If another constant or global variable with the same name exists, reports an error and
     * does not add the function to the namespace.
     * \param f the function to add
     */
    void addFunction(Function::Ptr f);

    /**
     * \brief Adds a global variable to this namespace.
     *
     * If another constant, function or global variable with the same name exists, reports an error and
     * does not add the global variable to the namespace.
     * \param gv the global variable to add
     */
    void addGlobalVariable(GlobalVariable::Ptr gv);

private:
    Namespace(Context &context, Namespace *parent, SourceLocation location, std::string name)
            : context(context), parent(parent), location(location), name(std::move(name)) {
    }

    bool findPreviousMember(const std::string &name, SourceLocation &location) const;

    Context &context;
    Namespace *parent;
    SourceLocation location;
    std::string name;
    std::map<std::string, Namespace::Ptr> namespaces;
    std::map<std::string, std::unique_ptr<Class>> classes;
    std::map<std::string, Constant::Ptr> constants;
    std::map<std::string, Function::Group> functionGroups;
    std::map<std::string, GlobalVariable::Ptr> globalVariables;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_NAMESPACE_H_
