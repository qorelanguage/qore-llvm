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

bool Namespace::findPreviousMember(const std::string &name, SourceLocation &location) const {
    auto it1 = constants.find(name);
    if (it1 != constants.end()) {
        location = it1->second->getLocation();
        return true;
    }
    auto it2 = functionGroups.find(name);
    if (it2 != functionGroups.end()) {
        location = it2->second[0]->getLocation();
        return true;
    }
    auto it3 = globalVariables.find(name);
    if (it3 != globalVariables.end()) {
        location = it3->second->getLocation();
        return true;
    }
    return false;
}

void Namespace::addConstant(Constant::Ptr c) {
    SourceLocation old;
    if (findPreviousMember(c->getName(), old)) {
        context.report(DiagId::SemaDuplicateConstantName, c->getLocation()) << c->getName() << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, old);
        return;
    }
    constants[c->getName()] = std::move(c);
}

void Namespace::addFunction(Function::Ptr f) {
    auto it = functionGroups.find(f->getName());
    if (it != functionGroups.end()) {
        it->second.push_back(std::move(f));
        return;
    }

    SourceLocation old;
    if (findPreviousMember(f->getName(), old)) {
        context.report(DiagId::SemaDuplicateFunctionName, f->getLocation()) << f->getName() << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, old);
        return;
    }
    functionGroups[f->getName()].push_back(std::move(f));
}

void Namespace::addGlobalVariable(GlobalVariable::Ptr gv) {
    SourceLocation old;
    if (findPreviousMember(gv->getName(), old)) {
        context.report(DiagId::SemaDuplicateGlobalVariableName, gv->getLocation()) << gv->getName() << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, old);
        return;
    }
    globalVariables[gv->getName()] = std::move(gv);
}

} // namespace semantic
} // namespace comp
} // namespace qore
