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
#include "qore/comp/ast/Evaluator.h"
#include "qore/common/Exceptions.h"

namespace qore {
namespace comp {
namespace semantic {

/// \cond NoDoxygen
class TypeResolver {

public:
    TypeResolver(TypeRegistry &typeRegistry, Scope &scope) : typeRegistry(typeRegistry), scope(scope) {
    }

    Type::Ref eval(ast::NameType &node) {
        return typeRegistry.resolveName(scope, node.name, false);
    }

    Type::Ref eval(ast::AsteriskType &node) {
        return typeRegistry.resolveName(scope, node.name, true);
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
    return ast::evaluateTypeNode<Type::Ref>(node, resolver);
}

Type::Ref TypeRegistry::resolveName(Scope &scope, const ast::Name &name, bool asterisk) {
    if (!name.isValid()) {
        return Type::Ref();
    }
    if (name.isSimple()) {
        if (Type *t = findBuiltin(context.getIdentifier(name.last()))) {
            if (asterisk) {
                QORE_UNREACHABLE("Not implemented");
            }
            return Type::Ref(t);
        }
    }
    QORE_UNREACHABLE("Not implemented");
//    scope->resolve(node.name);
//    filter classes
//    0  - undeclared
//    2+ - ambiguous
//    1  - should be a class
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

} // namespace semantic
} // namespace comp
} // namespace qore
