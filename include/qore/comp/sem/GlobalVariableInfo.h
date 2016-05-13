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
/// \brief Defines the GlobalVariableInfo class.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_GLOBALVARIABLEINFO_H_
#define INCLUDE_QORE_COMP_SEM_GLOBALVARIABLEINFO_H_

#include <memory>
#include "qore/comp/sem/Core.h"
#include "qore/comp/ast/Namespace.h"
#include "qore/core/GlobalVariable.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Describes a global variable during semantic analysis.
 */
class GlobalVariableInfo {

public:
    using Ptr = std::unique_ptr<GlobalVariableInfo>;        //!< Pointer type.

public:
    /**
     * \brief Constructor.
     * \param core the shared state of the analyzer
     * \param parent the parent namespace scope
     * \param node the AST node
     */
    GlobalVariableInfo(Core &core, NamespaceScope &parent, ast::GlobalVariable &node)
            : core(core), parent(parent), node(node), rt(nullptr) {
    }

    /**
     * \brief Returns the location of the global variable declaration.
     * \return the location of the global variable declaration
     */
    SourceLocation getLocation() const {
        return node.name.last().location;
    }

    /**
     * \brief Returns the type of the global variable.
     *
     * This method can be called only if \ref pass2() has been called.
     * \return the type of the global variable
     */
    const Type &getType() const {
        assert(rt != nullptr);
        return rt->getType();
    }

    /**
     * \brief Returns the runtime object representing the global variable.
     *
     * This method can be called only if \ref pass2() has been called.
     * \return the runtime object representing the global variable
     */
    GlobalVariable &getRt() const {
        assert(rt != nullptr);
        return *rt;
    }

    /**
     * \brief Resolves the type of the global variable.
     */
    void pass2();

private:
    Core &core;
    NamespaceScope &parent;
    ast::GlobalVariable &node;
    GlobalVariable *rt;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_GLOBALVARIABLEINFO_H_
