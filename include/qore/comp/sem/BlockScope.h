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
#ifndef INCLUDE_QORE_COMP_SEM_BLOCKSCOPE_H_
#define INCLUDE_QORE_COMP_SEM_BLOCKSCOPE_H_

#include <map>
#include <vector>
#include "qore/comp/sem/LocalVariableInfo.h"
#include "qore/comp/sem/Scope.h"
#include "qore/comp/sem/stmt/Statement.h"
#include "qore/comp/sem/stmt/CompoundStatement.h"

namespace qore {
namespace comp {
namespace sem {

class BlockScope : public Scope {

public:
    explicit BlockScope(Scope &parent) : parent(parent) {
    }

    LocalVariableInfo &createLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        return parent.createLocalVariable(name, location, type);
    }

    LocalVariableInfo &declareLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        if (locals.find(name) != locals.end()) {
            //FIXME report duplicate local variable in the same block
            QORE_NOT_IMPLEMENTED("");
        }
        LocalVariableInfo &lv = createLocalVariable(name, location, type);
        locals[name] = &lv;
        return lv;
    }

    const Type &resolveType(const ast::Type &node) const override {
        return parent.resolveType(node);
    }

    Symbol resolveSymbol(const ast::Name &name) const override {
        if (name.isSimple()) {
            auto it = locals.find(name.last().str);
            if (it != locals.end()) {
                return Symbol(*it->second);
            }
        }
        return parent.resolveSymbol(name);
    }

    const Type &getReturnType() const override {
        return parent.getReturnType();
    }

private:
    Scope &parent;
    std::map<String::Ref, LocalVariableInfo *> locals;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_BLOCKSCOPE_H_
