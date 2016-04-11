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
#include <string>
#include <vector>

namespace qore {
namespace cg {

ScriptCompiler::ScriptCompiler(Helper &helper, as::S &script) : helper(helper), bInit(helper.ctx), bDone(helper.ctx),
        strings(script.strings.size()), globals(script.globalCount) {
    llvm::Function *qinit = helper.createFunction("qinit", helper.lt_void);
    llvm::Function *qdone = helper.createFunction("qdone", helper.lt_void);

    bInit.SetInsertPoint(llvm::BasicBlock::Create(helper.ctx, "entry", qinit));
    bDone.SetInsertPoint(llvm::BasicBlock::Create(helper.ctx, "entry", qdone));

    bInit.SetInsertPoint(bInit.CreateRetVoid());
    bDone.SetInsertPoint(bDone.CreateRetVoid());

    as::Id id = 0;
    for (auto &s : script.strings) {
        addStringLiteral(id++, s);
    }

    //create functions before globals (they may be needed by initializer expressions)
    for (auto &f : script.functions) {
        if (f->name == "qmain") {
            functions[f.get()] = helper.createFunction("qmain", helper.lt_void);
        } else if (f->name == "qinit") {
            functions[f.get()] = qinit;
        } else if (f->name == "qdone") {
            functions[f.get()] = qdone;
        } else {
            QORE_NOT_IMPLEMENTED("unknown function " << f->name);
        }
    }

    for (auto &gv : globals) {
        gv = new llvm::GlobalVariable(*helper.module, helper.lt_GlobalVariable_ptr, false,
                llvm::GlobalValue::InternalLinkage, llvm::Constant::getNullValue(helper.lt_GlobalVariable_ptr));
    }
}

void ScriptCompiler::addStringLiteral(as::Id id, const std::string value) {
    llvm::Constant *val = llvm::ConstantDataArray::getString(helper.ctx, value, true);
    llvm::GlobalVariable *str = new llvm::GlobalVariable(*helper.module, val->getType(), true,
            llvm::GlobalValue::PrivateLinkage, val, llvm::Twine("str").concat(llvm::Twine(id)));
    str->setUnnamedAddr(true);
    llvm::GlobalVariable *lit = new llvm::GlobalVariable(*helper.module, helper.lt_qvalue, false,
            llvm::GlobalValue::InternalLinkage, llvm::Constant::getNullValue(helper.lt_qvalue),
            llvm::Twine("lit").concat(llvm::Twine(id)));
    llvm::Value *args[2];
    args[0] = bInit.CreateConstGEP2_32(nullptr, str, 0, 0);
    args[1] = llvm::ConstantInt::get(llvm::Type::getInt64Ty(helper.ctx), value.size());
    bInit.CreateStore(bInit.CreateCall(helper.lf_createString, args), lit);

    llvm::LoadInst *load = bDone.CreateLoad(lit);
    bDone.CreateCall(helper.lf_decRef, load);
    bDone.SetInsertPoint(load);

    strings[id] = lit;
}

class FunctionCompiler {

public:
    FunctionCompiler(as::F &f, llvm::Function *func, Helper &helper, std::vector<llvm::GlobalVariable *> &strings,
            std::vector<llvm::GlobalVariable *> &globals) : f(f), func(func), helper(helper), strings(strings),
            globals(globals), locals(f.localCount), temps(f.tempCount) {
        func->setPersonalityFn(llvm::ConstantExpr::getBitCast(helper.lf_personality, helper.lt_char_ptr));
    }

    void x() {
        llvm::IRBuilder<> builder(llvm::BasicBlock::Create(helper.ctx, "entry", func));
        for (auto &l : locals) {
            l = builder.CreateAlloca(helper.lt_qvalue, nullptr);    //FIXME name
        }
        excSlot = builder.CreateAlloca(helper.lt_exc, nullptr, "exc.slot");
        builder.CreateBr(doBlock(0));
    }

    llvm::BasicBlock *doBlock(as::Id i) {
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(helper.ctx, "bb", func);
        llvm::IRBuilder<> builder(bb);
        for (auto &ii : f.blocks[i]->instructions) {
            switch (ii->getKind()) {
                case as::Instruction::Kind::IntConstant: {
                    as::IntConstant &ins = static_cast<as::IntConstant &>(*ii);
                    temps[ins.getDest()] = builder.CreateCall(
                            helper.lf_qint_to_qvalue, llvm::ConstantInt::get(helper.lt_qint, ins.getValue(), true));
                    break;
                }
                case as::Instruction::Kind::GetLocal: {
                    as::GetLocal &ins = static_cast<as::GetLocal &>(*ii);
                    temps[ins.getDest()] = builder.CreateLoad(locals[ins.getSlot()]);
                    break;
                }
                case as::Instruction::Kind::SetLocal: {
                    as::SetLocal &ins = static_cast<as::SetLocal &>(*ii);
                    builder.CreateStore(temps[ins.getSrc()], locals[ins.getSlot()]);
                    break;
                }
                case as::Instruction::Kind::LoadString: {
                    as::LoadString &ins = static_cast<as::LoadString &>(*ii);
                    temps[ins.getDest()] = builder.CreateLoad(strings[ins.getId()]);
                    break;
                }
                case as::Instruction::Kind::RefInc: {
                    as::RefInc &ins = static_cast<as::RefInc &>(*ii);
                    builder.CreateCall(helper.lf_incRef, temps[ins.getTemp()]);
                    break;
                }
                case as::Instruction::Kind::RefDec: {
                    as::RefDec &ins = static_cast<as::RefDec &>(*ii);
                    if (ins.getLpad() != as::InvalidId) {
                        llvm::BasicBlock *bb = llvm::BasicBlock::Create(helper.ctx, "bb", func);
                        builder.CreateInvoke(helper.lf_decRef, bb, doBlock(ins.getLpad()), temps[ins.getTemp()]);
                        builder.SetInsertPoint(bb);
                    } else {
                        builder.CreateCall(helper.lf_decRef, temps[ins.getTemp()]);
                    }
                    break;
                }
                case as::Instruction::Kind::RefDecNoexcept: {
                    as::RefDecNoexcept &ins = static_cast<as::RefDecNoexcept &>(*ii);
                    llvm::Value *args[2] = { temps[ins.getTemp()], temps[ins.getE()] };
                    builder.CreateCall(helper.lf_decRefNoexcept, args);
                    break;
                }
                case as::Instruction::Kind::ReadLockGlobal: {
                    as::ReadLockGlobal &ins = static_cast<as::ReadLockGlobal &>(*ii);
                    builder.CreateCall(helper.lf_gv_read_lock, builder.CreateLoad(globals[ins.getId()]));
                    break;
                }
                case as::Instruction::Kind::ReadUnlockGlobal: {
                    as::ReadUnlockGlobal &ins = static_cast<as::ReadUnlockGlobal &>(*ii);
                    builder.CreateCall(helper.lf_gv_read_unlock, builder.CreateLoad(globals[ins.getId()]));
                    break;
                }
                case as::Instruction::Kind::WriteLockGlobal: {
                    as::WriteLockGlobal &ins = static_cast<as::WriteLockGlobal &>(*ii);
                    builder.CreateCall(helper.lf_gv_write_lock, builder.CreateLoad(globals[ins.getId()]));
                    break;
                }
                case as::Instruction::Kind::WriteUnlockGlobal: {
                    as::WriteUnlockGlobal &ins = static_cast<as::WriteUnlockGlobal &>(*ii);
                    builder.CreateCall(helper.lf_gv_write_unlock, builder.CreateLoad(globals[ins.getId()]));
                    break;
                }
                case as::Instruction::Kind::GetGlobal: {
                    as::GetGlobal &ins = static_cast<as::GetGlobal &>(*ii);
                    temps[ins.getDest()] = builder.CreateCall(helper.lf_gv_get,
                            builder.CreateLoad(globals[ins.getId()]));
                    break;
                }
                case as::Instruction::Kind::SetGlobal: {
                    as::SetGlobal &ins = static_cast<as::SetGlobal &>(*ii);
                    llvm::Value *args[2] = { builder.CreateLoad(globals[ins.getId()]), temps[ins.getSrc()] };
                    builder.CreateCall(helper.lf_gv_set, args);
                    break;
                }
                case as::Instruction::Kind::MakeGlobal: {
                    as::MakeGlobal &ins = static_cast<as::MakeGlobal &>(*ii);
                    builder.CreateStore(builder.CreateCall(helper.lf_gv_create, temps[ins.getSrc()]),
                            globals[ins.getId()]);
                    break;
                }
                case as::Instruction::Kind::FreeGlobal: {
                    as::FreeGlobal &ins = static_cast<as::FreeGlobal &>(*ii);
                    temps[ins.getDest()] = builder.CreateCall(helper.lf_gv_free,
                            builder.CreateLoad(globals[ins.getId()]));
                    break;
                }
                case as::Instruction::Kind::LandingPad: {
                    as::LandingPad &ins = static_cast<as::LandingPad &>(*ii);
                    llvm::LandingPadInst *l = builder.CreateLandingPad(helper.lt_exc, 1);
                    l->setCleanup(true);
                    builder.CreateStore(l, excSlot);
                    temps[ins.getDest()] = llvm::Constant::getNullValue(helper.lt_qvalue);  //FIXME extract exception
                    break;
                }
                case as::Instruction::Kind::Rethrow: {
                    builder.CreateResume(builder.CreateLoad(excSlot));
                    break;
                }
                case as::Instruction::Kind::BinaryOperator: {
                    as::BinaryOperator &ins = static_cast<as::BinaryOperator &>(*ii);
                    llvm::Value *args[2] = { temps[ins.getLeft()], temps[ins.getRight()] };
                    if (ins.getLpad() != as::InvalidId) {
                        llvm::BasicBlock *bb = llvm::BasicBlock::Create(helper.ctx, "bb", func);
                        temps[ins.getDest()] = builder.CreateInvoke(helper.binOpFunctions[&ins.getDesc()],
                                bb, doBlock(ins.getLpad()), args);
                        builder.SetInsertPoint(bb);
                    } else {
                        temps[ins.getDest()] = builder.CreateCall(helper.binOpFunctions[&ins.getDesc()], args);
                    }
                    break;
                }
                case as::Instruction::Kind::Conversion: {
                    as::Conversion &ins = static_cast<as::Conversion &>(*ii);
                    if (ins.getLpad() != as::InvalidId) {
                        llvm::BasicBlock *bb = llvm::BasicBlock::Create(helper.ctx, "bb", func);
                        temps[ins.getDest()] = builder.CreateInvoke(helper.convFunctions[&ins.getDesc()],
                                bb, doBlock(ins.getLpad()), temps[ins.getArg()]);
                        builder.SetInsertPoint(bb);
                    } else {
                        temps[ins.getDest()] = builder.CreateCall(helper.convFunctions[&ins.getDesc()],
                                temps[ins.getArg()]);
                    }
                    break;
                }
                case as::Instruction::Kind::RetVoid:
                    builder.CreateRetVoid();
                    break;
                default:
                    QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ii->getKind()));
            }
        }
        return bb;
    }


private:
    as::F &f;
    llvm::Function *func;
    Helper &helper;
    std::vector<llvm::GlobalVariable *> &strings;
    std::vector<llvm::GlobalVariable *> &globals;
    std::vector<llvm::AllocaInst *> locals;
    std::vector<llvm::Value *> temps;
    llvm::Value *excSlot;
};

void ScriptCompiler::compile(as::F &f) {
    FunctionCompiler fc(f, functions[&f], helper, strings, globals);
    fc.x();
}

} // namespace cg
} // namespace qore
