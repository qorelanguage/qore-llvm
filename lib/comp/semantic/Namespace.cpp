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
#include "qore/comp/semantic/Namespace.h"
#include <vector>
#include "ReportedError.h"

namespace qore {
namespace comp {
namespace semantic {

Namespace &Namespace::addNamespace(String::Ref name, SourceLocation location) {
    if (Namespace *ns = findNamespace(name)) {
        return *ns;
    }
    if (Class *c = findClass(name)) {
        ctx.report(DiagId::SemaDuplicateNamespaceName, location) << name << getFullName();
        ctx.report(DiagId::SemaPreviousDeclaration, c->getLocation());
        throw ReportedError();
    }
    return add<Namespace>(*this, name, location);
}

Class &Namespace::addClass(String::Ref name, SourceLocation location) {
    Class *old = findClass(name);
    Namespace *ns = findNamespace(name);
    if (old || ns) {
        ctx.report(DiagId::SemaDuplicateClassName, location) << name << getFullName();
        ctx.report(DiagId::SemaPreviousDeclaration, old ? old->getLocation() : ns->getLocation());
        throw ReportedError();
    }
    return add<Class>(ctx, *this, name, location);
}

GlobalVariable &Namespace::addGlobalVariable(String::Ref name, SourceLocation location, Type::Ref type) {
    for (auto &s : symbols) {
        if (s->getName() == name) {
            if (s->getKind() == Kind::GlobalVariable) { //TODO or function or constant
                ctx.report(DiagId::SemaDuplicateGlobalVariableName, location) << name << getFullName();
                ctx.report(DiagId::SemaPreviousDeclaration, s->getLocation());
                throw ReportedError();
            }
        }
    }
    return add<GlobalVariable>(ctx, *this, name, location, type);
}

Class &Namespace::resolveClass(const ast::Name &name) const {
    assert(name.isValid());
    if (name.isRoot()) {
        if (Class *c = getRoot().lookupClass(name.begin(), name.end())) {
            return *c;
        }
        ctx.report(DiagId::SemaUnresolvedClass, name.getStart()) << name;
        throw ReportedError();
    }
    if (parent) {
        if (Class *c = lookupClass(name.begin(), name.end())) {
            return *c;
        }
        return parent->resolveClass(name);
    }
    std::vector<Class *> classes;
    collectClasses(classes, name.begin(), name.end());
    if (classes.size() == 1) {
        return *classes[0];
    }
    ctx.report(classes.empty() ? DiagId::SemaUnresolvedClass : DiagId::SemaAmbiguousClass, name.getStart()) << name;
    throw ReportedError();
}

Class *Namespace::lookupClass(ast::Name::Iterator begin, ast::Name::Iterator end) const {
    assert(begin != end);

    if (begin + 1 == end) {
        return findClass(begin->str);
    }
    if (Namespace *ns = findNamespace(begin->str)) {
        return ns->lookupClass(begin + 1, end);
    }
    return nullptr;
}

void Namespace::collectClasses(std::vector<Class *> &classes, ast::Name::Iterator begin,
        ast::Name::Iterator end) const {
    assert(begin != end);

    if (begin + 1 == end) {
        if (Class *c = findClass(begin->str)) {
            classes.push_back(c);
        }
    } else if (Namespace *ns = findNamespace(begin->str)) {
        if (Class *c = ns->lookupClass(begin + 1, end)) {
            classes.push_back(c);
        }
    }
    for (auto &it : symbols) {
        if (it->getKind() == Kind::Namespace) {
            static_cast<Namespace &>(*it).collectClasses(classes, begin, end);
        }
    }
}

} // namespace semantic
} // namespace comp
} // namespace qore
