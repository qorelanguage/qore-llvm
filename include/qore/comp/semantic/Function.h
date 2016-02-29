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
/// \brief Representation of functions during semantic analysis.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_FUNCTION_H_
#define INCLUDE_QORE_COMP_SEMANTIC_FUNCTION_H_

#include <string>
#include <vector>
#include "qore/comp/semantic/Context.h"
#include "qore/comp/ast/Namespace.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Represents a function.
 */
class Function {

public:
    using Ptr = std::unique_ptr<Function>;                  //!< Pointer type.

public:
    /**
     * \brief Creates a new instance.
     * \param context the context for semantic analysis
     * \param astNode the AST node, the name must be valid
     * \return a unique pointer to the allocated instance
     */
    static Ptr create(Context &context, ast::Function &astNode) {
        assert(astNode.name.isValid());
        return Ptr(new Function(context, astNode));
    }

    /**
     * \brief Returns the location of the function declaration.
     * \return the location of the function declaration.
     */
    const SourceLocation &getLocation() const {
        return location;
    }

    /**
     * \brief Returns the name of the function.
     * \return the name of the function
     */
    const std::string getName() const {
        return name;
    }

private:
    Function(Context &context, ast::Function &astNode) : context(context), location(astNode.name.last().location),
            astNode(*astNode.routine), name(context.getIdentifier(astNode.name.last())) {
    }

private:
    Context &context;
    SourceLocation location;
    ast::Routine &astNode;
    std::string name;
};

/**
 * \brief Represents a group of function overloads.
 */
class FunctionGroup : public NamedObjectBase<NamedObject::Kind::FunctionGroup> {

public:
    using Ptr = std::unique_ptr<FunctionGroup>;             //!< Pointer type.

public:
    /**
     * \brief Creates a new function group containing given function.
     * \param f the function
     * \return a pointer to the new group
     */
    static Ptr create(Function::Ptr f) {
        assert(f);
        return Ptr(new FunctionGroup(std::move(f)));
    }

    const SourceLocation &getLocation() const override {
        return functions[0]->getLocation();
    }

    /**
     * \brief Adds a function to the group.
     * \param f thre function to add
     */
    void add(Function::Ptr f) {
        assert(f);
        functions.push_back(std::move(f));
    }

    //XXX begin(), end()
    std::vector<Function::Ptr>::const_iterator begin() const {
        return functions.begin();
    }

    std::vector<Function::Ptr>::const_iterator end() const {
        return functions.end();
    }

private:
    explicit FunctionGroup(Function::Ptr f) {
        functions.push_back(std::move(f));
    }

private:
    std::vector<Function::Ptr> functions;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_FUNCTION_H_
