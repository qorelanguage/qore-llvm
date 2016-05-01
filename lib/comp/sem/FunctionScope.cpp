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
#include "qore/comp/sem/FunctionScope.h"
#include <string>
#include "qore/comp/sem/FunctionOverloadPack.h"
#include "qore/comp/sem/Builder.h"
#include "StatementAnalyzerPass1.h"
#include "StatementAnalyzerPass2.h"

namespace qore {
namespace comp {
namespace sem {

FunctionScope::FunctionScope(Function &rt, Core &core, Scope &parent, ast::Routine &node)
        : rt(rt), core(core), parent(parent), node(node) {
}

const Type &FunctionScope::resolveType(ast::Type &node) const {
    return parent.resolveType(node);
}

LocalVariable &FunctionScope::createLocalVariable(String::Ref name, SourceLocation location, const Type &type) {
    std::unique_ptr<LocalVariable> ptr = util::make_unique<LocalVariable>(name, location, type);
    LocalVariable &lv = *ptr;
    locals.push_back(std::move(ptr));
    return lv;
}

Symbol FunctionScope::resolveSymbol(ast::Name &name) const {
    if (name.isSimple()) {
        auto it = args.find(name.last().str);
        if (it != args.end()) {
            return Symbol(*it->second);
        }
    }
    return parent.resolveSymbol(name);
}

void FunctionScope::analyze() {

    for (auto &a : node.params) {
        const Type &type = resolveType(std::get<0>(a)); //XXX parameter types have already been resolved
        args[std::get<1>(a).str] = &createLocalVariable(std::get<1>(a).str, std::get<1>(a).location, type);
    }

    Statement::Ptr body = core.doPass1(*this, *node.body);

    Builder b;
    Id index = 0;
    for (auto it = node.params.begin(); it != node.params.end(); ++it) {
        LocalVariable &lv = *args[std::get<1>(*it).str];
        b.startOfArgumentLifetime(lv, index++);
    }

    StatementAnalyzerPass2 a2(core, b);
    body->accept(a2);

    if (!b.isTerminated()) {
        if (getReturnType() == Type::Nothing) {
            b.createRetVoid();
        } else {
            QORE_UNREACHABLE("");   //missing return statement
        }
    }

    b.build(rt);
}

} // namespace sem
} // namespace comp
} // namespace qore
