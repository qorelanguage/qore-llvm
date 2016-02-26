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
Namespace *Namespace::findOrCreateNamespace(const Token &token) {
    LOG_FUNCTION();
    std::string name = context.getIdentifier(token);
    if (Namespace *ns = findNamespace(name)) {
        return ns;
    }
    if (Class *c = findClass(name)) {
        context.report(DiagId::SemaDuplicateNamespaceName, token.location) << name << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, c->getLocation());
        return nullptr;
    }
    Namespace::Ptr ptr = Ptr(new Namespace(this, token.location, name));
    Namespace *value = ptr.get();
    namespaces[name] = std::move(ptr);
    return value;
}

Class *Namespace::createClass(const Token &token) {
    LOG_FUNCTION();
    std::string name = context.getIdentifier(token);
    Class *c = findClass(name);
    Namespace *ns = findNamespace(name);
    if (c || ns) {
        context.report(DiagId::SemaDuplicateClassName, token.location) << name << getFullName();
        context.report(DiagId::SemaPreviousDeclaration, c ? c->getLocation() : ns->getLocation());
        return nullptr;
    }
    Class::Ptr ptr = Class::create(context, *this, token.location, name);
    Class *value = ptr.get();
    classes[name] = std::move(ptr);
    return value;
}

} // namespace semantic
} // namespace comp
} // namespace qore