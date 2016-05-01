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
#ifndef INCLUDE_QORE_COMP_SEM_FUNCTIONSCOPE_H_
#define INCLUDE_QORE_COMP_SEM_FUNCTIONSCOPE_H_

#include <map>
#include <string>
#include <vector>
#include "qore/comp/sem/Core.h"
#include "qore/comp/ast/Routine.h"
#include "qore/comp/sem/Scope.h"
#include "qore/core/Function.h"

namespace qore {
namespace comp {
namespace sem {

//resolves parameters
//knows the type of the return value
//owns local variables
//helps with return/throw statement

class FunctionOverloadPack;
class FunctionScope : public Scope {

public:
    using Ptr = std::unique_ptr<FunctionScope>;

public:
    FunctionScope(Function &rt, Core &core, Scope &parent, ast::Routine &node);


    const Type &resolveType(ast::Type &node) const override;
    LocalVariable &createLocalVariable(String::Ref name, SourceLocation location, const Type &type) override;
    Symbol resolveSymbol(ast::Name &name) const override;
    LocalVariable &declareLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        QORE_UNREACHABLE("");
    }

    const Type &getReturnType() const override {
        return rt.getType().getReturnType();
    }

    void analyze();

    Function &getRt() const {
        return rt;
    }

private:
    Function &rt;
    Core &core;
    Scope &parent;
    ast::Routine &node;
    std::vector<std::unique_ptr<LocalVariable>> locals;
    std::map<String::Ref, LocalVariable *> args;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_FUNCTIONSCOPE_H_
