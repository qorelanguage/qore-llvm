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
#ifndef LIB_CG_FUNCTIONCOMPILER_H_
#define LIB_CG_FUNCTIONCOMPILER_H_

#include <map>
#include <string>
#include <vector>
#include "qore/core/Function.h"
#include "qore/core/code/Block.h"
#include "Helper.h"

namespace qore {
namespace cg {

class FunctionContext {

public:
    FunctionContext(Helper &helper, Size localCount, Size tempCount) : helper(helper), locals(localCount),
            temps(tempCount), excSlot(nullptr) {
    }

public:
    Helper &helper;
    std::vector<llvm::AllocaInst *> locals;
    std::vector<llvm::Value *> temps;
    llvm::Value *excSlot;
    std::map<const code::Block *, llvm::BasicBlock *> blockMap;
    std::vector<const code::Block *> queue;
};

class FunctionCompiler {

public:
    FunctionCompiler(const Function &f, llvm::Function *func, Helper &helper) : f(f), func(func),
            ctx(helper, f.getLocalVariables().size(), f.getTempCount()) {
        func->setPersonalityFn(llvm::ConstantExpr::getBitCast(helper.lf_personality, helper.lt_char_ptr));
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(helper.ctx, "entry", func);
        llvm::IRBuilder<> builder(entry);

        for (const LocalVariable &lv : f.getLocalVariables()) {
            ctx.locals[lv.getIndex()] = builder.CreateAlloca(helper.lt_qvalue, nullptr, lv.getName());
        }
        ctx.excSlot = builder.CreateAlloca(helper.lt_exc, nullptr, "exc.slot");

        Index i = 0;
        for (auto it = func->arg_begin(); it != func->arg_end(); ++it) {
            it->setName(llvm::Twine(ctx.locals[i]->getName()).concat(llvm::Twine(".val")));
            builder.CreateStore(it, ctx.locals[i++]);
        }

        ctx.blockMap[&f.getEntryBlock()] = entry;
        ctx.queue.push_back(&f.getEntryBlock());
    }

    void compile() {
        while (!ctx.queue.empty()) {
            const code::Block *b = ctx.queue.back();
            ctx.queue.pop_back();
            doBlock(*b);
        }
        //func->viewCFG();
    }

private:
    void doBlock(const code::Block &block);

private:
    const Function &f;
    llvm::Function *func;
    FunctionContext ctx;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_FUNCTIONCOMPILER_H_
