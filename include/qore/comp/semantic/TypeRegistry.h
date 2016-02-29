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
#ifndef INCLUDE_QORE_COMP_SEMANTIC_TYPEREGISTRY_H_
#define INCLUDE_QORE_COMP_SEMANTIC_TYPEREGISTRY_H_

#include <map>
#include <string>
#include "qore/comp/semantic/Context.h"
#include "qore/comp/semantic/Scope.h"
#include "qore/comp/semantic/Type.h"
#include "qore/comp/ast/Type.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Repository of all known types.
 */
class TypeRegistry {

public:
    /**
     * \brief Constructor.
     * \param context the context for semantic analysis
     */
    explicit TypeRegistry(Context &context) : context(context) {
    }

    /**
     * \brief Resolves a type represented by an AST node.
     * \param scope current scope for resolving names
     * \param node the AST node
     * \return reference to the resolved type
     */
    Type::Ref resolve(Scope &scope, ast::Type::Ptr &node);

private:
    Type::Ref resolveName(Scope &scope, const ast::Name &name);
    Type *findBuiltin(const std::string &name);
    Type::Ref getImplicit() const {
        return Type::Ref(&builtinImplicit);
    }
    Type::Ref getAsteriskType(Type::Ref t);

private:
    Context &context;
    BuiltinType builtinImplicit{"<implicit>"};
    BuiltinType builtinInt{"int"};
    BuiltinType builtinString{"string"};
    std::map<const Class *, ClassType::Ptr> classTypes;
    std::map<const Type *, AsteriskType::Ptr> asteriskTypes;

    friend class TypeResolver;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_TYPEREGISTRY_H_
