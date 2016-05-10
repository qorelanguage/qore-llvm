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

const Type &FunctionScope::resolveType(const ast::Type &node) const {
    return parent.resolveType(node);
}

LocalVariableInfo &FunctionScope::createLocalVariable(String::Ref name, SourceLocation location, const Type &type) {
    std::unique_ptr<LocalVariableInfo> ptr = util::make_unique<LocalVariableInfo>(name, location, type);
    LocalVariableInfo &lv = *ptr;
    locals.push_back(std::move(ptr));
    return lv;
}

Symbol FunctionScope::resolveSymbol(const ast::Name &name) const {
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

    FBuilder b(rt);

    for (auto &lv : locals) {
        //change type if lv is shared
        lv->setRt(rt.addLocalVariable(core.ctx.getString(lv->getName()), lv->getType(), lv->getLocation()));
    }

    for (auto &p : args) {
        LocalVariableInfo &lv = *p.second;

        //if not shared:
        if (lv.getType().isRefCounted()) {
            code::Temp temp = b.getFreeTemp();
            b.createLocalGet(temp, lv);
            b.createRefInc(temp);
            b.setTempFree(temp);
            b.pushCleanup(lv);
        }

        //if shared:
        // - get value from lv to temp
        // - if not primitive, emit refInc
        // - emit instruction for allocating the wrapper with 'temp' as the initial value
        // - emit instruction for storing the wrapper ptr to local slot aslv
        // - push cleanup scope that dereferences the wrapper

        //both compiler and interpreter need to copy the function arguments into local slots (starting from 0)
    }

    StatementAnalyzerPass2::analyze(core, b, *body);

    if (!b.isTerminated()) {
        if (getReturnType() == Type::Nothing) {
            b.createRetVoid();
        } else {
            QORE_UNREACHABLE("");   //missing return statement
        }
    }
}

} // namespace sem
} // namespace comp
} // namespace qore
