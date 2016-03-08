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

#include <string>
#include <vector>
#include "qore/comp/Context.h"
#include "qore/comp/semantic/Class.h"
#include "qore/comp/semantic/GlobalVariable.h"
#include "qore/comp/semantic/Scope.h"
#include "qore/comp/semantic/Symbol.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Represents a namespace.
 */
class Namespace : public NamedScope, public Symbol {

public:
    /**
     * \brief Creates the root namespace.
     * \param ctx the compiler context
     */
    explicit Namespace(Context &ctx) : ctx(ctx), parent(nullptr), name() {
    }

    /**
     * \brief Creates a new namespace.
     * \param parent the parent namespace
     * \param name the name of the namespace
     * \param location the location of the first declaration of the namespace
     */
    Namespace(Namespace &parent, String::Ref name, SourceLocation location)
            : ctx(parent.ctx), parent(&parent), name(name), location(location) {
    }

    Kind getKind() const override {
        return Kind::Namespace;
    }

    String::Ref getName() const override {
        assert(!isRoot());
        return name;
    }

    SourceLocation getLocation() const override {
        assert(!isRoot());
        return location;
    }

    std::string getFullName() const override {
        return parent ? parent->getFullName() + "::" + ctx.getStringTable().get(name) : "";
    }

    Class &resolveClass(const ast::Name &name) const override;

    /**
     * \brief Return true if this instance represents the root namespace.
     * \return true if this instance represents the root namespace
     */
    bool isRoot() const {
        return parent == nullptr;
    }

    /**
     * \brief Adds a namespace to this namespace.
     *
     * If another class with the same name exists, reports an error and throws ReportedError.
     * If another namespace with the same name exists, returns it instead of creating a new instance.
     * \param name the name of the namespace
     * \param location the location of the declaration
     * \return namespace object
     * \throws ReportedError if a class with given name already exists
     */
    Namespace &addNamespace(String::Ref name, SourceLocation location);

    /**
     * \brief Adds a class to this namespace.
     *
     * If another class or namespace with the same name exists, reports an error and throws ReportedError.
     * \param name the name of the class
     * \param location the location of the declaration
     * \return class object
     * \throws ReportedError if a class or namespace with given name already exists
     */
    Class &addClass(String::Ref name, SourceLocation location);

    /**
     * \brief Adds a global variable to this namespace.
     *
     * If another constant, function or global variable with the same name exists, reports an error and
     * throws ReportedError.
     * \param name the name of the variable
     * \param location the location of the declaration
     * \param type the type of the variable
     * \return global variable object
     * \throws ReportedError if a constant, function or global variable with given name already exists
     */
    GlobalVariable &addGlobalVariable(String::Ref name, SourceLocation location, Type::Ref type);

    /**
     * \brief Finds a namespace with given name.
     * \param name the name to find
     * \return the namespace or `nullptr` if not found
     */
    Namespace *findNamespace(String::Ref name) const {
        //FIXME symbol table?
        for (const Symbol::Ptr &s : symbols) {
            if (s->getName() == name && s->getKind() == Kind::Namespace) {
                return static_cast<Namespace *>(s.get());
            }
        }
        return nullptr;
    }

    /**
     * \brief Finds a class with given name.
     * \param name the name to find
     * \return the class or `nullptr` if not found
     */
    Class *findClass(String::Ref name) const {
        //FIXME symbol table?
        for (const Symbol::Ptr &s : symbols) {
            if (s->getName() == name && s->getKind() == Kind::Class) {
                return static_cast<Class *>(s.get());
            }
        }
        return nullptr;
    }

private:
    //FIXME symbol table?
    template<typename S, typename... Args>
    S &add(Args&&... args) {
        std::unique_ptr<S> uptr(new S(std::forward<Args>(args)...));
        S &ptr = *uptr;
        symbols.push_back(std::move(uptr));
        return ptr;
    }

    const Namespace &getRoot() const {
        const Namespace *ns = this;
        while (ns->parent) {
            ns = ns->parent;
        }
        return *ns;
    }

    Class *lookupClass(ast::Name::Iterator begin, ast::Name::Iterator end) const;
    void collectClasses(std::vector<Class *> &classes, ast::Name::Iterator begin, ast::Name::Iterator end) const;


private:
    Context &ctx;
    Namespace *parent;
    String::Ref name;
    SourceLocation location;
    std::vector<Symbol::Ptr> symbols;

    template<typename OS> friend class Dump;      //TODO remove friend Dump
};

} // namespace semantic

/**
 * \brief Sets the value of a parameter of the message.
 *
 * The value will replace the next occurrence of '%s' in the message.
 * \param b the diagnostic message builder
 * \param ns the value of the parameter
 */
template<>
inline void toDiagArg(DiagBuilder &b, const semantic::Namespace &ns) {
    if (ns.isRoot()) {
        b << "the root namespace";
    } else {
        b << "namespace " + ns.getFullName();
    }
}

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_NAMESPACE_H_
