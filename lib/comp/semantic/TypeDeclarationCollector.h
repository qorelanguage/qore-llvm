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
#ifndef LIB_COMP_SEMANTIC_TYPEDECLARATIONCOLLECTOR_H_
#define LIB_COMP_SEMANTIC_TYPEDECLARATIONCOLLECTOR_H_

#include "qore/comp/ast/VisitorBase.h"
#include "qore/comp/semantic/Context.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief AST visitor that collects all type declarations.
 *
 * The goal of this pass is to collect all declarations of types, which are currently only classes. In order to
 * correctly build their full names, namespace declarations need to be processed as well.
 */
class TypeDeclarationCollector: public ast::DeclarationVisitorBase {

public:
    /**
     * \brief Constructor.
     * \param context the context for semantic analysis
     * \param currentNamespace the current namespace, must not be `nullptr`
     */
    TypeDeclarationCollector(Context &context, Namespace *currentNamespace) : context(context),
            currentNamespace(currentNamespace) {
        assert(currentNamespace != nullptr);
    }

    void visit(ast::Script &node) override {
        for (ast::NamespaceMember::Ptr &m : node.members) {
            m->accept(*this);
        }
    }

    void visit(ast::Namespace &node) override {
        LOG_FUNCTION();
        if (!node.name.isValid()) {
            return;
        }
        if (node.name.isRoot()) {
            context.report(DiagId::SemaInvalidNamespaceName, node.name.getStart());
            return;
        }
        if (Namespace *parent = findParentFor(node.name)) {
            if (Namespace *ns = parent->findOrCreateNamespace(node.name.last())) {
                TypeDeclarationCollector nc(context, ns);
                for (ast::NamespaceMember::Ptr &m : node.members) {
                    m->accept(nc);
                }
            }
        }
    }

    void visit(ast::Class &node) override {

    }

private:
    Namespace *findParentFor(const ast::Name &name) {
        Namespace *parent = currentNamespace;
        for (ast::Name::Iterator it = name.begin(); it != name.end() - 1; ++it) {
            Namespace *ns = parent->findNamespace(*it);
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

#endif // LIB_COMP_SEMANTIC_TYPEDECLARATIONCOLLECTOR_H_
