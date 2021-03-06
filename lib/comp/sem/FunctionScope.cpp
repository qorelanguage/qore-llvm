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
/// \brief Implements the FunctionScope class.
///
//------------------------------------------------------------------------------
#include "qore/comp/sem/FunctionScope.h"
#include <string>
#include <vector>
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
    LocalVariableInfo::Ptr ptr = LocalVariableInfo::Ptr(new LocalVariableInfo(name, location, type));
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

void FunctionScope::analyze(std::vector<Statement::Ptr> *initializers) {
    Index index = 0;
    for (auto &a : node.params) {
        String::Ref name = std::get<1>(a).str;
        SourceLocation location = std::get<1>(a).location;
        const Type &type = rt.getType().getParameterType(index++);
        args[name] = &createLocalVariable(name, location, type);
    }

    Statement::Ptr body = StatementAnalyzerPass1::analyze(core, *this, *node.body);

    FunctionBuilder b(rt);

    for (auto &lv : locals) {
        //change type if lv is shared
        lv->setRt(rt.addLocalVariable(core.getString(lv->getName()), lv->getType(), lv->getLocation()));
    }

    for (auto &p : args) {
        LocalVariableInfo &lv = *p.second;

        //if not shared:
        if (lv.getType().isRefCounted()) {
            TempHelper temp(b);
            b.createLocalGet(temp, lv);
            b.createRefInc(temp);
            b.localsPush(lv);
        }

        //if shared:
        // - get value from lv to temp
        // - if not primitive, emit refInc
        // - emit instruction for allocating the wrapper with 'temp' as the initial value
        // - emit instruction for storing the wrapper ptr to local slot aslv
        // - builder.localPush() that dereferences the wrapper

        //both compiler and interpreter need to copy the function arguments into local slots (starting from 0)
    }

    if (initializers) {
        for (auto &stmt : *initializers) {
            StatementAnalyzerPass2::analyze(core, b, *stmt);
        }
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
