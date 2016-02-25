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

#include "qore/common/Util.h"
#include "qore/comp/semantic/Context.h"
#include "qore/comp/semantic/Symbol.h"
#include "qore/comp/ast/Name.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Represents a namespace.
 */
class Namespace : public SymbolBase<Symbol::Kind::Namespace> {

public:
    /**
     * \brief Creates the root namespace.
     * \param context the context for semantic analysis
     * \return the root namespace
     */
    static std::unique_ptr<Namespace> createRoot(Context &context) {
        return util::make_unique<Namespace>(context);
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

    /**
     * \brief Returns the full name of the namespace.
     * \return the full name of the namespace
     */
    std::string getFullName() const {
        return (parent ? (parent->isRoot() ? "" : parent->getFullName()) + "::" : "") + name;
    }

    /**
     * \brief Calls a function for each member of the namespace.
     * \param callback the callback to call
     */
    void forEach(std::function<void(Symbol *)> callback) const {
        for (auto &s : members) {
            callback(s.get());
        }
    }

    /**
     * \brief Finds a namespace with given name.
     * \param token an identifier token representing the name to find
     * \return the namespace or `nullptr` if not found
     */
    Namespace *findNamespace(const Token &token) const {
        return symbolTable.find<Namespace>(context.getIdentifier(token));
    }

    /**
     * \brief Finds a namespace with given name or creates it if it does not exist.
     * \param token an identifier token representing the name of the namespace
     * \return a pointer to the namespace or `nullptr` if a class with given name already exists
     */
    Namespace *findOrCreateNamespace(const Token &token);

    /**
     * \brief Creates a class with given name.
     * \param token an identifier token representing the name of the class
     * \return a pointer to the class or `nullptr` if a class or namespace with given name already exists
     */
    class Class *createClass(const Token &token);

    /**
     * \brief Finds namespace for a declaration with given name.
     * \param name the name of the declaration (namespace, class, constant, ...)
     * \return the parent namespace for given name
     */
    Namespace *findParentFor(const ast::Name &name);

private:
    explicit Namespace(Context &context) : context(context), parent(nullptr), name("<root>") {
    }

    Namespace(Namespace *parent, SourceLocation location, std::string name) : context(parent->context), parent(parent),
            location(location), name(std::move(name)) {
    }

    Context &context;
    Namespace *parent;
    SourceLocation location;
    std::string name;
    std::vector<std::unique_ptr<Symbol>> members;
    SymbolTable symbolTable;

    FRIEND_MAKE_UNIQUE;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_NAMESPACE_H_
