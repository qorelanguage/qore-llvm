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
/// \brief Defines the ClassScope class.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_CLASSSCOPE_H_
#define INCLUDE_QORE_COMP_SEM_CLASSSCOPE_H_

#include <memory>
#include <string>
#include <utility>
#include "qore/core/Class.h"
#include "qore/comp/ast/Class.h"
#include "qore/comp/sem/Scope.h"

namespace qore {
namespace comp {
namespace sem {

class NamespaceScope;

/**
 * \brief Describes a class during semantic analysis and implements its scope.
 */
class ClassScope {

public:
    using Ptr = std::unique_ptr<ClassScope>;    //!< Pointer type.

public:
    /**
     * \brief Constructor.
     * \param rt the runtime object representing the class
     * \param core the shared state of the analyzer
     * \param parent the parent namespace
     * \param node the AST node
     */
    ClassScope(Class &rt, Core &core, NamespaceScope &parent, ast::Class &node) : rt(rt), core(core),
            parent(parent), node(node) {
//        for (auto &decl : node.members) {
//            try {processDeclaration(*decl); } catch (ReportedError &) { ignore }
//        }
    }

    /**
     * \brief Returns the runtime object representing the class.
     * \return the runtime object representing the class
     */
    Class &getRt() {
        return rt;
    }

private:
    Class &rt;
    Core &core;
    NamespaceScope &parent;
    ast::Class &node;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_CLASSSCOPE_H_
