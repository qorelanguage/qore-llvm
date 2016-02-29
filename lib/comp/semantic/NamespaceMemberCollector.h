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
/// \brief AST visitor that collects all type declarations.
///
//------------------------------------------------------------------------------
#ifndef LIB_COMP_SEMANTIC_NAMESPACEMEMBERCOLLECTOR_H_
#define LIB_COMP_SEMANTIC_NAMESPACEMEMBERCOLLECTOR_H_

#include <string>
#include "qore/comp/ast/VisitorBase.h"
#include "qore/comp/semantic/Class.h"
#include "qore/comp/semantic/Namespace.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief AST visitor that collects all declarations of namespace members.
 *
 * The goal of this pass is to assign all declarations to correct namespace regardless of the syntax used:
 * \code
 * namespace N {
 *     our int a;
 * }
 * out int N::b;
 * \endcode
 * In the example, both global variables are members of the same namespace.
 * This cuts the original AST into pieces and puts them together in a different hierarchy which represents
 * the actual scoping instead of the original textual order.
 */
class NamespaceMemberCollector: public ast::NamespaceMemberVisitor {

public:
    /**
     * \brief Collects all namespace member declarations from a script and returns the root namespace.
     * \param context the context for semantic analysis
     * \param script the AST script node
     * \return the root namespace
     */
    static Namespace::Ptr collect(Context &context, ast::Script::Ptr &script) {
        Namespace::Ptr rootNamespace = Namespace::createRoot(context);
        NamespaceMemberCollector visitor(context, rootNamespace.get());
        for (ast::NamespaceMember::Ptr &m : script->members) {
            m->accept(visitor);
        }
        //TODO synthesize global function main from the top level statements
        return rootNamespace;
    }

    void visit(ast::Namespace &node) override {
        LOG_FUNCTION();
        if (Namespace *parent = findParentFor(node.name)) {
            std::string name = context.getIdentifier(node.name.last());
            Namespace *ns = parent->findNamespace(name);
            if (!ns) {
                Namespace::Ptr ptr = Namespace::create(*parent, node.name.last().location, std::move(name));
                ns = ptr.get();
                if (!parent->addNamespace(std::move(ptr))) {
                    return;
                }
            }
            Namespace *prevNamespace = currentNamespace;
            currentNamespace = ns;
            for (ast::NamespaceMember::Ptr &m : node.members) {
                m->accept(*this);
            }
            currentNamespace = prevNamespace;
        }
    }

    void visit(ast::Class &node) override {
        LOG_FUNCTION();
        if (Namespace *parent = findParentFor(node.name)) {
            parent->addClass(Class::create(context, *parent, node));
        }
    }

    void visit(ast::NamespaceConstant &node) override {
        LOG_FUNCTION();
        if (Namespace *parent = findParentFor(node.constant->name)) {
            parent->addConstant(Constant::create(context, *currentNamespace, *node.constant));
        }
    }

    void visit(ast::Function &node) override {
        LOG_FUNCTION();
        if (Namespace *parent = findParentFor(node.name)) {
            parent->addFunction(Function::create(context, node));
        }
    }

    void visit(ast::GlobalVariable &node) override {
        LOG_FUNCTION();
        if (Namespace *parent = findParentFor(node.name)) {
            parent->addGlobalVariable(GlobalVariable::create(context, *currentNamespace, node));
        }
    }

private:
    NamespaceMemberCollector(Context &context, Namespace *currentNamespace)
            : context(context), currentNamespace(currentNamespace) {
    }

    Namespace *findParentFor(const ast::Name &name) {
        if (!name.isValid()) {
            return nullptr;
        }
        if (name.isRoot()) {
            context.report(DiagId::SemaInvalidNamespaceMemberName, name.getStart());
            return nullptr;
        }
        Namespace *parent = currentNamespace;
        for (ast::Name::Iterator it = name.begin(); it != name.end() - 1; ++it) {
            Namespace *ns = parent->findNamespace(context.getIdentifier(*it));
            if (!ns) {
                context.report(DiagId::SemaNamespaceNotFound, it->location)
                        << context.getIdentifier(*it)
                        << parent->getFullName();
                return nullptr;
            }
            parent = ns;
        }
        return parent;
    }

private:
    Context &context;
    Namespace *currentNamespace;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEMANTIC_NAMESPACEMEMBERCOLLECTOR_H_
