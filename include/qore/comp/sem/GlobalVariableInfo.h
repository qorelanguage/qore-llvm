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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_GLOBALVARIABLEINFO_H_
#define INCLUDE_QORE_COMP_SEM_GLOBALVARIABLEINFO_H_

#include <string>
#include "qore/comp/sem/Core.h"
#include "qore/comp/ast/Namespace.h"
#include "qore/core/GlobalVariable.h"

namespace qore {
namespace comp {
namespace sem {

/*
 * pass1 (constructor)
 *      - getName
 *      - getLocation
 *      - getFullName
 *      - parent
 * pass2
 */
class GlobalVariableInfo {

public:
    using Ptr = std::unique_ptr<GlobalVariableInfo>;

public:
    GlobalVariableInfo(Core &core, NamespaceScope &parent, ast::GlobalVariable &node)
            : core(core), parent(parent), node(node), rt(nullptr) {
    }

    SourceLocation getLocation() const {
        return node.name.last().location;
    }

    const Type &getType() const {
        assert(rt != nullptr);
        return rt->getType();
    }

    GlobalVariable &getRt() const {
        assert(rt != nullptr);
        return *rt;
    }

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
