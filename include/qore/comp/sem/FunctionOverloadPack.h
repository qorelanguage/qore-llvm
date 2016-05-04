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

#include <utility>
#include <vector>
#include "qore/comp/sem/FunctionScope.h"
#include "qore/core/FunctionGroup.h"

namespace qore {
namespace comp {
namespace sem {

class FunctionOverloadPack {

public:
    using Ptr = std::unique_ptr<FunctionOverloadPack>;

public:
    FunctionOverloadPack(FunctionGroup &rt, Core &core, Scope &parent, SourceLocation location)
            : rt(rt), core(core), parent(parent), location(location) {
    }

    SourceLocation getLocation() const {
        return location;
    }

    void addOverload(ast::Routine &node, SourceLocation location) {
        if (queue.empty()) {
            core.addToQueue(*this);
        }
        queue.emplace_back(&node, location);
    }

    void pass2();

    Scope &getParent() {
        return parent;
    }

private:
    void checkOverload(FunctionType &type);

private:
    FunctionGroup &rt;
    Core &core;
    Scope &parent;
    SourceLocation location;
    std::vector<std::pair<ast::Routine *, SourceLocation>> queue;
    std::vector<FunctionScope::Ptr> functions;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_FUNCTIONOVERLOADPACK_H_
