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
#include "ScriptCompiler.h"
#include "StatementCompiler.h"
#include "ExpressionCompiler.h"

namespace qore {
namespace cg {

ScriptCompiler::ScriptCompiler(Helper &helper) : helper(helper), bInit(helper.ctx), bDone(helper.ctx) {
    llvm::Function *qinit = helper.createFunction("qinit", helper.lt_void);
    llvm::Function *qdone = helper.createFunction("qdone", helper.lt_void);

    bInit.SetInsertPoint(llvm::BasicBlock::Create(helper.ctx, "entry", qinit));
    bDone.SetInsertPoint(llvm::BasicBlock::Create(helper.ctx, "entry", qdone));

    bInit.SetInsertPoint(bInit.CreateRetVoid());
    bDone.SetInsertPoint(bDone.CreateRetVoid());
}

void ScriptCompiler::addStringLiteral(const ir::StringLiteral &sl) {
    qore::rt::qsize id = strings.size();
    llvm::Constant *val = llvm::ConstantDataArray::getString(helper.ctx, sl.getValue(), true);
    llvm::GlobalVariable *str = new llvm::GlobalVariable(*helper.module, val->getType(), true,
            llvm::GlobalValue::PrivateLinkage, val, llvm::Twine("str").concat(llvm::Twine(id)));
    str->setUnnamedAddr(true);
    llvm::GlobalVariable *lit = new llvm::GlobalVariable(*helper.module, helper.lt_qptr, false,
            llvm::GlobalValue::InternalLinkage, llvm::Constant::getNullValue(helper.lt_qptr),
            llvm::Twine("lit").concat(llvm::Twine(id)));
    llvm::Value *args[2];
    args[0] = bInit.CreateConstGEP2_32(nullptr, str, 0, 0);
    args[1] = llvm::ConstantInt::get(llvm::Type::getInt64Ty(helper.ctx), sl.getValue().size());
    bInit.CreateStore(bInit.CreateCall(helper.lf_createString, args), lit);

    llvm::LoadInst *load = bDone.CreateLoad(lit);
    bDone.CreateCall(helper.lf_decRef, load);
    bDone.SetInsertPoint(load);

    strings[&sl] = lit;
}

void ScriptCompiler::addGlobalVariable(const ir::GlobalVariable &gv) {
    llvm::GlobalVariable *g = new llvm::GlobalVariable(*helper.module, helper.lt_GlobalVariable_ptr, false,
            llvm::GlobalValue::InternalLinkage, llvm::Constant::getNullValue(helper.lt_GlobalVariable_ptr),
            gv.getName());
    //who deallocates it?

    Locals locals;
    ExpressionCompiler expressionCompiler(*this, helper, locals, bInit);
    Value initValue = expressionCompiler.compile(gv.getInitExpression());
    assert(initValue.type == gv.getType().rtType);
    llvm::Value *gg = bInit.CreateCall(helper.lf_gv_create,
            helper.toQvalue(bInit, initValue.value, gv.getType().rtType));
    bInit.CreateStore(gg, g);

    llvm::LoadInst *load = bDone.CreateLoad(g);
    llvm::Value *val = bDone.CreateCall(helper.lf_gv_free, load);

    if (gv.getType().rtType == qore::rt::qvalue_type::Ptr) {
        bDone.CreateCall(helper.lf_decRef, bDone.CreateCall(helper.lf_qvalue_to_qptr, val));
    }
    bDone.SetInsertPoint(load);

    globals[&gv] = g;
}

void ScriptCompiler::addFunction(const ir::UserFunction &f) {
    if (f.getName() == "qmain") {
        functions[&f] = helper.createFunction("qmain", helper.lt_void);
    } else {
        QORE_NOT_IMPLEMENTED("unknown function " << f.getName());
    }
}

void ScriptCompiler::compile(const ir::UserFunction &f) {
    Locals locals;

    llvm::Function *func = functions[&f];

    llvm::IRBuilder<> builder(llvm::BasicBlock::Create(helper.ctx, "entry", func));

    for (auto &lv : f.getLocalVariables()) {
        locals.add(*lv, builder.CreateAlloca(helper.mapType(lv->getType().rtType), nullptr, lv->getName()));
    }

    StatementCompiler statementCompiler(*this, helper, locals, builder);
    for (auto &stmt : f.getStatements()) {
        statementCompiler.compile(*stmt);
    }

    //these should be in the intermediate code explicitly
    for (auto &lv : f.getLocalVariables()) {
        if (lv->getType().rtType == rt::qvalue_type::Ptr) {
            builder.CreateCall(helper.lf_decRef, builder.CreateLoad(locals.get(*lv)));
        }
    }
    builder.CreateRetVoid();
}

} // namespace cg
} // namespace qore
