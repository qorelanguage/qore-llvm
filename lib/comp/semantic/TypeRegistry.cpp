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
/// \brief Repository of all known types.
///
//------------------------------------------------------------------------------
#include "qore/comp/semantic/TypeRegistry.h"
#include <string>
#include "qore/common/Exceptions.h"
#include "qore/comp/ast/Evaluator.h"
#include "qore/comp/semantic/Class.h"

namespace qore {
namespace comp {
namespace semantic {

std::string ClassType::getName() const {
    return clazz.getFullName();
}

/// \cond NoDoxygen
class TypeResolver {

public:
    TypeResolver(TypeRegistry &typeRegistry, Scope &scope) : typeRegistry(typeRegistry), scope(scope) {
    }

    Type::Ref eval(ast::NameType &node) {
        return typeRegistry.resolveName(scope, node.name);
    }

    Type::Ref eval(ast::AsteriskType &node) {
        return typeRegistry.getAsteriskType(typeRegistry.resolveName(scope, node.name));
    }

    Type::Ref eval(ast::ImplicitType &node) {
        return typeRegistry.getImplicit();
    }

private:
    TypeRegistry &typeRegistry;
    Scope &scope;
};
/// \endcond NoDoxygen

Type::Ref TypeRegistry::resolve(Scope &scope, ast::Type::Ptr &node) {
    TypeResolver resolver(*this, scope);
    return ast::evaluateTypeNode<Type::Ref>(*node, resolver);
}

Type::Ref TypeRegistry::resolveName(Scope &scope, const ast::Name &name) {
    if (!name.isValid()) {
        return Type::Ref();
    }
    if (name.isSimple()) {
        if (Type *t = findBuiltin(context.getIdentifier(name.last()))) {
            return Type::Ref(t);
        }
    }
    Class *c = scope.resolveClass(name);
    if (!c) {
        return Type::Ref();
    }
    ClassType::Ptr &t = classTypes[c];
    if (!t) {
        t = ClassType::create(*c);
    }
    return Type::Ref(t.get());
}

Type *TypeRegistry::findBuiltin(const std::string &name) {
    if (name == "int") {
        return &builtinInt;
    }
    if (name == "string") {
        return &builtinString;
    }
    return nullptr;
}

Type::Ref TypeRegistry::getAsteriskType(Type::Ref t) {
    if (!t.type || t.type == getImplicit().type) {     //XXX any
        return t;
    }

    AsteriskType::Ptr &p = asteriskTypes[t.type];
    if (!p) {
        p = AsteriskType::create(*t.type);
    }
    return Type::Ref(p.get());
}

} // namespace semantic
} // namespace comp
} // namespace qore
