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
#include <string>
#include "qore/comp/semantic/Class.h"

namespace qore {
namespace comp {
namespace semantic {

//TODO modifiers
//TODO reserved words
bool Namespace::addNamespace(Ptr ns) {
    assert(!findNamespace(ns->getName()));
    if (Class *c = findClass(ns->getName())) {
        context.report(DiagId::SemaDuplicateNamespaceName, ns->getLocation()) << ns->getName() << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, c->getLocation());
        return false;
    }
    namespaces[ns->getName()] = std::move(ns);
    return true;
}

void Namespace::addClass(Class::Ptr c) {
    Class *old = findClass(c->getName());
    Namespace *ns = findNamespace(c->getName());
    if (old || ns) {
        context.report(DiagId::SemaDuplicateClassName, c->getLocation()) << c->getName() << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, old ? old->getLocation() : ns->getLocation());
        return;
    }
    classes[c->getName()] = std::move(c);
}

void Namespace::addConstant(Constant::Ptr c) {
    NamedObject::Ptr &p = objects[c->getName()];
    if (!p) {
        p = std::move(c);
    } else {
        context.report(DiagId::SemaDuplicateConstantName, c->getLocation()) << c->getName() << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, p->getLocation());
    }
}

void Namespace::addFunction(Function::Ptr f) {
    NamedObject::Ptr &p = objects[f->getName()];
    if (!p) {
        p = FunctionGroup::create(std::move(f));
    } else if (p->getKind() == NamedObject::Kind::FunctionGroup) {
        static_cast<FunctionGroup &>(*p).add(std::move(f));
    } else {
        context.report(DiagId::SemaDuplicateFunctionName, f->getLocation()) << f->getName() << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, p->getLocation());
    }
}

void Namespace::addGlobalVariable(GlobalVariable::Ptr gv) {
    NamedObject::Ptr &p = objects[gv->getName()];
    if (!p) {
        p = std::move(gv);
    } else {
        context.report(DiagId::SemaDuplicateGlobalVariableName, gv->getLocation()) << gv->getName() << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, p->getLocation());
    }
}

} // namespace semantic
} // namespace comp
} // namespace qore
