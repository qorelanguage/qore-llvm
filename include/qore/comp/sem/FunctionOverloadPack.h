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
#ifndef INCLUDE_QORE_COMP_SEM_FUNCTIONOVERLOADPACK_H_
#define INCLUDE_QORE_COMP_SEM_FUNCTIONOVERLOADPACK_H_

#include <vector>
#include "qore/common/Util.h"
#include "qore/comp/sem/FunctionScope.h"

namespace qore {
namespace comp {
namespace sem {

class FunctionOverloadPack {

public:
    using Ptr = std::unique_ptr<FunctionOverloadPack>;

public:
    FunctionOverloadPack(Core &core, NamespaceScope &parent, String::Ref name, SourceLocation location)
            : core(core), parent(parent), name(name), location(location) {
    }

    String::Ref getName() const {
        return name;
    }

    SourceLocation getLocation() const {
        return location;
    }

    void addOverload(ast::Routine &node) {
        if (queue.empty()) {
            core.addToQueue(*this);
        }
        queue.push_back(&node);
    }

    void pass2();

private:
    void checkOverload(FunctionScope &f1);

private:
    Core &core;
    NamespaceScope &parent;
    String::Ref name;
    SourceLocation location;
    std::vector<ast::Routine *> queue;
    std::vector<FunctionScope::Ptr> functions;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_FUNCTIONOVERLOADPACK_H_
