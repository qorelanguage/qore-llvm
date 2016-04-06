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

#include <unordered_map>
#include "qore/ir/decl/Function.h"
#include "qore/ir/decl/GlobalVariable.h"
#include "qore/ir/decl/StringLiteral.h"
#include "Helper.h"

namespace qore {
namespace cg {

class ScriptCompiler {

public:
    explicit ScriptCompiler(Helper &helper);

    llvm::GlobalVariable *getString(const ir::StringLiteral &sl) const {
        auto it = strings.find(&sl);
        assert(it != strings.end());
        return it->second;
    }

    llvm::GlobalVariable *getGlobal(const ir::GlobalVariable &gv) const {
        auto it = globals.find(&gv);
        assert(it != globals.end());
        return it->second;
    }

    llvm::Value *call(llvm::IRBuilder<> builder, const ir::Function &f, llvm::ArrayRef<llvm::Value *> args) const {
        llvm::Value *v = helper.call(builder, f, args);
        if (v) {
            return v;
        }
        auto it = functions.find(&f);
        assert(it != functions.end());
        return builder.CreateCall(it->second, args);
    }

    void addStringLiteral(const ir::StringLiteral &sl);
    void addGlobalVariable(const ir::GlobalVariable &gv);
    void addFunction(const ir::UserFunction &f);
    void compile(const ir::UserFunction &f);

private:
    Helper &helper;
    llvm::IRBuilder<> bInit;
    llvm::IRBuilder<> bDone;

    std::unordered_map<const ir::StringLiteral *, llvm::GlobalVariable *> strings;
    std::unordered_map<const ir::GlobalVariable *, llvm::GlobalVariable *> globals;
    std::unordered_map<const ir::Function *, llvm::Function *> functions;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_SCRIPTCOMPILER_H_
