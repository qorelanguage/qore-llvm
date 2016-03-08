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
/// \brief Semantic analyzer.
///
//------------------------------------------------------------------------------
#include "qore/comp/semantic/Analyzer.h"
#include <vector>
#include "ReportedError.h"

namespace qore {
namespace comp {
namespace semantic {

void Analyzer::collectNamespacesAndClasses(Namespace &ns, std::vector<ast::Declaration::Ptr> &declNodes) {
    for (ast::Declaration::Ptr &declNode : declNodes) {
        try {
            if (declNode->getKind() == ast::Declaration::Kind::Namespace) {
                createNamespace(ns, static_cast<ast::Namespace &>(*declNode));
            } else if (declNode->getKind() == ast::Declaration::Kind::Class) {
                createClass(ns, static_cast<ast::Class &>(*declNode));
            } else {
                nsMemberQueue.emplace_back(&ns, declNode.get());
            }
        } catch (ReportedError &) {
            // ignore declaration
        }
    }
}

void Analyzer::declareNamespaceMembers() {
    for (auto &i : nsMemberQueue) {
        try {
            if (i.second->getKind() == ast::Declaration::Kind::GlobalVariable) {
                createGlobalVariable(*i.first, static_cast<ast::GlobalVariable &>(*i.second));
            } else {
                //TODO functions, constants
            }
        } catch (ReportedError &) {
            // ignore declaration
        }
    }
}

void Analyzer::processClasses() {
    //TODO classes
//    for (auto &i : classQueue) {
//    }
}

void Analyzer::createNamespace(Namespace &scope, ast::Namespace &node) {
    Namespace &parent = findParentFor(scope, node.name);
    //TODO check modifiers, reserved words
    Namespace &ns = parent.addNamespace(node.name.last().str, node.name.last().location);
    collectNamespacesAndClasses(ns, node.members);
}

void Analyzer::createClass(Namespace &scope, ast::Class &node) {
    Namespace &parent = findParentFor(scope, node.name);
    //TODO check modifiers, reserved words
    classQueue.emplace_back(&parent.addClass(node.name.last().str, node.name.last().location), &node);
}

void Analyzer::createGlobalVariable(Namespace &scope, ast::GlobalVariable &node) {
    Namespace &parent = findParentFor(scope, node.name);
    //TODO check modifiers, reserved words
    Type::Ref type = resolveType(scope, node.type);
    parent.addGlobalVariable(node.name.last().str, node.name.last().location, type);
}

Namespace &Analyzer::findParentFor(Namespace &scope, const ast::Name &name) {
    if (!name.isValid()) {
        throw ReportedError();
    }
    if (name.isRoot()) {
        ctx.report(DiagId::SemaInvalidNamespaceMemberName, name.getStart());
        throw ReportedError();
    }
    Namespace *parent = &scope;
    for (ast::Name::Iterator it = name.begin(); it != name.end() - 1; ++it) {
        if (Namespace *ns = parent->findNamespace(it->str)) {
            parent = ns;
        } else {
            ctx.report(DiagId::SemaNamespaceNotFound, it->location) << it->str << *parent;
            throw ReportedError();
        }
    }
    return *parent;
}

} // namespace semantic
} // namespace comp
} // namespace qore
