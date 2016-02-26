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
/// \brief Representation of global variables during semantic analysis.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_GLOBALVARIABLE_H_
#define INCLUDE_QORE_COMP_SEMANTIC_GLOBALVARIABLE_H_

#include <string>
#include "qore/comp/semantic/Context.h"
#include "qore/comp/semantic/TypeRegistry.h"
#include "qore/comp/ast/Namespace.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Represents a global variable.
 */
class GlobalVariable {

public:
    using Ptr = std::unique_ptr<GlobalVariable>;            //!< Pointer type.

public:
    /**
     * \brief Creates a new instance.
     * \param context the context for semantic analysis
     * \param scope the parent scope
     * \param astNode the AST node, the name must be valid
     * \return a unique pointer to the allocated instance
     */
    static Ptr create(Context &context, NamedScope &scope, ast::GlobalVariable &astNode) {
        assert(astNode.name.isValid());
        return Ptr(new GlobalVariable(context, scope, astNode));
    }

    /**
     * \brief Returns the location of the global variable declaration.
     * \return the location of the global variable declaration.
     */
    const SourceLocation &getLocation() const {
        return astNode.name.last().location;
    }

    /**
     * \brief Returns the name of the global variable.
     * \return the name of the global variable
     */
    const std::string getName() const {
        return name;
    }

    /**
     * \brief Returns the type of the global variable.
     * \return the type of the global variable
     */
    const Type::Ref &getType() const {
        return type;
    }

    /**
     * \brief Returns the full name of the global variable.
     * \return the full name of the global variable
     */
    const std::string getFullName() const {
        return scope.getFullName() + "::" + name;
    }

    /**
     * \brief Resolves the type of the global variable.
     */
    void analyzeType(TypeRegistry &typeRegistry) {
        LOG("Analyzing type of global variable " << getFullName());
        type = typeRegistry.resolve(scope, astNode.type);
    }

private:
    GlobalVariable(Context &context, NamedScope &scope, ast::GlobalVariable &astNode) : context(context),
            scope(scope), astNode(astNode), name(context.getIdentifier(astNode.name.last())) {
    }

private:
    Context &context;               //XXX context is not needed
    NamedScope &scope;
    ast::GlobalVariable &astNode;
    std::string name;
    Type::Ref type;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_GLOBALVARIABLE_H_
