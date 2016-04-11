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
#ifndef LIB_CG_SCRIPTCOMPILER_H_
#define LIB_CG_SCRIPTCOMPILER_H_

#include <string>
#include <vector>
#include "qore/ir/decl/Function.h"
#include "qore/ir/decl/GlobalVariable.h"
#include "qore/ir/decl/StringLiteral.h"
#include "qore/as/as.h"
#include "Helper.h"

namespace qore {
namespace cg {

class ScriptCompiler {

public:
    ScriptCompiler(Helper &helper, as::S &script);
    void compile(as::F &f);

private:
    void addStringLiteral(as::Id id, const std::string value);

    void doBlock();

private:
    Helper &helper;
    llvm::IRBuilder<> bInit;
    llvm::IRBuilder<> bDone;

    std::vector<llvm::GlobalVariable *> strings;
    std::vector<llvm::GlobalVariable *> globals;
    std::unordered_map<as::F *, llvm::Function *> functions;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_SCRIPTCOMPILER_H_
