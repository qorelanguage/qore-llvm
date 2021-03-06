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
/// \brief Defines the NamespaceScope class.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_NAMESPACESCOPE_H_
#define INCLUDE_QORE_COMP_SEM_NAMESPACESCOPE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "qore/core/Namespace.h"
#include "qore/comp/Context.h"
#include "qore/comp/ast/Class.h"
#include "qore/comp/ast/Namespace.h"
#include "qore/comp/sem/ClassScope.h"
#include "qore/comp/sem/FunctionGroupInfo.h"
#include "qore/comp/sem/GlobalVariableInfo.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Describes a namespace during semantic analysis and implements its scope.
 */
class NamespaceScope : public Scope {

public:
    using Ptr = std::unique_ptr<NamespaceScope>;        //!< Pointer type.

public:
    /**
     * \brief Creates a root namespace scope.
     * \param core the shared state of the analyzer
     * \param rt the runtime object representing the root namespace
     */
    NamespaceScope(Core &core, Namespace &rt) : core(core), rt(rt), parentNamespace(nullptr) {
    }

    /**
     * \brief Creates a namespace scope.
     * \param rt the runtime object representing the namespace
     * \param parentNamespace the parent namespace scope
     */
    NamespaceScope(Namespace &rt, NamespaceScope &parentNamespace)
            : core(parentNamespace.core), rt(rt), parentNamespace(&parentNamespace) {
    }

    /**
     * \brief Returns the runtime object representing the namespace.
     * \return the runtime object representing the namespace
     */
    Namespace &getRt() {
        return rt;
    }

    /**
     * \brief Processes a declaration lexically nested in this namespace.
     *
     * Finds the semantic parent for the declaration according to the name in the AST node and calls its
     * processXxxDeclaration of appropriate type.
     * \param decl the AST node of the declaration
     */
    void processDeclaration(ast::Declaration &decl);

    const Type &resolveType(const ast::Type &node) const override;

    Symbol resolveSymbol(const ast::Name &name) const override;

    LocalVariableInfo &createLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        //this will happen if a local variable is declared in a constant initializer - report error and return
        //a fake local variable of type error which will need to be statically allocated somewhere
        QORE_NOT_IMPLEMENTED("");
    }

    LocalVariableInfo &declareLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        QORE_UNREACHABLE("");
    }

    const Type &getReturnType() const override {
        QORE_UNREACHABLE("");
    }

private:
    NamespaceScope &findParentFor(const ast::Name &name);

    /**
     * \brief Adds a namespace to this namespace.
     *
     * If another class with the same name exists, reports an error and throws ReportedError.
     * If another namespace with the same name exists, adds new declarations into that namespace instead
     * of creating a new one.
     * \param node the namespace declaration AST node
     * \throws ReportedError if a class with given name already exists
     */
    void processNamespaceDeclaration(ast::Namespace &node);

    /**
     * \brief Adds a class to this namespace.
     *
     * If another class or namespace with the same name exists, reports an error and throws ReportedError.
     * \param node the class declaration AST node
     * \throws ReportedError if a class or namespace with given name already exists
     */
    void processClassDeclaration(ast::Class &node);

    /**
     * \brief Adds a global variable to this namespace.
     *
     * If another constant, function or global variable with the same name exists, reports an error and
     * throws ReportedError.
     * \param node the global variable declaration AST node
     * \throws ReportedError if a constant, function or global variable with given name already exists
     */
    void processGlobalVariableDeclaration(ast::GlobalVariable &node);

    /**
     * \brief Adds a function to this namespace.
     *
     * If another constant, or global variable with the same name exists, reports an error and
     * throws ReportedError.
     * \param node the function declaration AST node
     * \throws ReportedError if a constant, function or global variable with given name already exists
     */
    void processFunctionDeclaration(ast::Function &node);

    /**
     * \brief Finds a namespace with given name directly contained in this namespace.
     * \param name the name to find
     * \return the namespace or `nullptr` if not found
     */
    NamespaceScope *findNamespace(String::Ref name) const {
        auto it = namespaces.find(name);
        return it == namespaces.end() ? nullptr : it->second.get();
    }

    /**
     * \brief Finds a class with given name.
     * \param name the name to find
     * \return the class or `nullptr` if not found
     */
    ClassScope *findClass(String::Ref name) const {
        auto it = classes.find(name);
        return it == classes.end() ? nullptr : it->second.get();
    }

    /**
     * \brief Finds a global variable with given name.
     * \param name the name to find
     * \return the global variable or `nullptr` if not found
     */
    GlobalVariableInfo *findGlobalVariable(String::Ref name) const {
        auto it = globalVariables.find(name);
        return it == globalVariables.end() ? nullptr : it->second.get();
    }

    /**
     * \brief Finds a function group with given name.
     * \param name the name to find
     * \return the function group or `nullptr` if not found
     */
    FunctionGroupInfo *findFunctionGroup(String::Ref name) const {
        auto it = functions.find(name);
        return it == functions.end() ? nullptr : it->second.get();
    }

    ClassScope &resolveClass(const ast::Name &name) const;
    ClassScope *lookupClass(ast::Name::Iterator begin, ast::Name::Iterator end) const;
    void collectClasses(std::vector<ClassScope *> &classes, ast::Name::Iterator begin, ast::Name::Iterator end) const;

    const NamespaceScope &getRoot() const {
        const NamespaceScope *ns = this;
        while (ns->parentNamespace) {
            ns = ns->parentNamespace;
        }
        return *ns;
    }

    std::string getNameForDiag() const {
        return parentNamespace == nullptr ? "the root namespace" : ("namespace " + rt.getFullName());
    }

private:
    Core &core;
    Namespace &rt;
    NamespaceScope *parentNamespace;
    std::map<String::Ref, Ptr> namespaces;
    std::map<String::Ref, ClassScope::Ptr> classes;
    std::map<String::Ref, GlobalVariableInfo::Ptr> globalVariables;
    std::map<String::Ref, FunctionGroupInfo::Ptr> functions;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_NAMESPACESCOPE_H_
