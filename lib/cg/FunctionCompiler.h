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

class FunctionCompiler {

public:
    FunctionCompiler(const Function &f, llvm::Function *func, Helper &helper, llvm::GlobalVariable *rtctx)
            : f(f), func(func), helper(helper), rtctx(rtctx), locals(f.getLocalVariables().size()),
              temps(f.getTempCount()), excSlot(nullptr) {
        func->setPersonalityFn(llvm::ConstantExpr::getBitCast(helper.lf_personality, helper.lt_char_ptr));
    }

    void x() {
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(helper.ctx, "entry", func);
        llvm::IRBuilder<> builder(entry);
        for (auto &l : locals) {
            l = builder.CreateAlloca(helper.lt_qvalue, nullptr);    //FIXME name
        }
        auto it = func->arg_begin();
        Index i = 0;
        while (it != func->arg_end()) {
            builder.CreateStore(it, locals[i++]);
        }

        excSlot = builder.CreateAlloca(helper.lt_exc, nullptr, "exc.slot");

        blockMap[&f.getEntryBlock()] = entry;
        queue.push_back(&f.getEntryBlock());
        while (!queue.empty()) {
            const code::Block *b = queue.back();
            queue.pop_back();
            doBlock(blockMap[b], *b);
        }
        //func->viewCFG();
    }

    llvm::Value *makeCallOrInvoke(llvm::IRBuilder<> &builder, const code::Instruction &ins,
            llvm::Function *f, llvm::ArrayRef<llvm::Value *> args) {
        if (!ins.getLpad()) {
            return builder.CreateCall(f, args);
        }
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(helper.ctx, "bb", func);


        llvm::BasicBlock *lpad = blockMap[ins.getLpad()];
        if (!lpad) {
            lpad = llvm::BasicBlock::Create(helper.ctx, "lpad", func);
            blockMap[ins.getLpad()] = lpad;

            llvm::IRBuilder<> bld(lpad);
            llvm::LandingPadInst *l = bld.CreateLandingPad(helper.lt_exc, 1);
            l->setCleanup(true);
            bld.CreateStore(l, excSlot);
            //FIXME extract exception and use it in decRefNoExcept
            doBlock(lpad, *ins.getLpad());
        }
        llvm::Value *ret = builder.CreateInvoke(f, bb, lpad, args);
        builder.SetInsertPoint(bb);
        return ret;
    }

    void doBlock(llvm::BasicBlock *bb, const code::Block &block) {
        llvm::IRBuilder<> builder(bb);
        for (const code::Instruction &ii : block) {
            switch (ii.getKind()) {
                case code::Instruction::Kind::Branch: {
                    const code::Branch &ins = static_cast<const code::Branch &>(ii);
                    builder.CreateCondBr(
                            builder.CreateCall(helper.lf_qvalue_to_qbool, temps[ins.getCondition().getIndex()]),
                            mapBlock(ins.getTrueDest(), "if.true"),
                            mapBlock(ins.getFalseDest(), "if.false"));
                    break;
                }
                case code::Instruction::Kind::ConstInt: {
                    const code::ConstInt &ins = static_cast<const code::ConstInt &>(ii);
                    temps[ins.getDest().getIndex()] = builder.CreateCall(
                            helper.lf_qint_to_qvalue, llvm::ConstantInt::get(helper.lt_qint, ins.getValue(), true));
                    break;
                }
                case code::Instruction::Kind::ConstString: {
                    const code::ConstString &ins = static_cast<const code::ConstString &>(ii);
                    temps[ins.getDest().getIndex()] = helper.loadString(ins.getString());
                    break;
                }
                case code::Instruction::Kind::InvokeBinaryOperator: {
                    const code::InvokeBinaryOperator &ins = static_cast<const code::InvokeBinaryOperator &>(ii);
                    llvm::Value *args[2] = { temps[ins.getLeft().getIndex()], temps[ins.getRight().getIndex()] };
                    temps[ins.getDest().getIndex()] = makeCallOrInvoke(builder, ins,
                            helper.getBinaryOperator(ins.getOperator()), args);
                    break;
                }
                case code::Instruction::Kind::InvokeConversion: {
                    const code::InvokeConversion &ins = static_cast<const code::InvokeConversion &>(ii);
                    temps[ins.getDest().getIndex()] = makeCallOrInvoke(builder, ins,
                            helper.getConversion(ins.getConversion()), temps[ins.getArg().getIndex()]);
                    break;
                }
                case code::Instruction::Kind::Jump: {
                    const code::Jump &ins = static_cast<const code::Jump &>(ii);
                    builder.CreateBr(mapBlock(ins.getDest(), "jump.dest"));
                    break;
                }
                case code::Instruction::Kind::LocalGet: {
                    const code::LocalGet &ins = static_cast<const code::LocalGet &>(ii);
                    temps[ins.getDest().getIndex()] = builder.CreateLoad(locals[ins.getLocalVariable().getIndex()]);
                    break;
                }
                case code::Instruction::Kind::LocalSet: {
                    const code::LocalSet &ins = static_cast<const code::LocalSet &>(ii);
                    builder.CreateStore(temps[ins.getSrc().getIndex()], locals[ins.getLocalVariable().getIndex()]);
                    break;
                }
                case code::Instruction::Kind::RefDec: {
                    const code::RefDec &ins = static_cast<const code::RefDec &>(ii);
                    makeCallOrInvoke(builder, ins, helper.lf_decRef, temps[ins.getTemp().getIndex()]);
                    break;
                }
                case code::Instruction::Kind::RefDecNoexcept: {
                    const code::RefDecNoexcept &ins = static_cast<const code::RefDecNoexcept &>(ii);
                    auto e = llvm::Constant::getNullValue(helper.lt_qvalue); //FIXME top of exception stack
                    llvm::Value *args[2] = { temps[ins.getTemp().getIndex()], e };
                    builder.CreateCall(helper.lf_decRefNoexcept, args);
                    break;
                }
                case code::Instruction::Kind::RefInc: {
                    const code::RefInc &ins = static_cast<const code::RefInc &>(ii);
                    builder.CreateCall(helper.lf_incRef, temps[ins.getTemp().getIndex()]);
                    break;
                }
                case code::Instruction::Kind::ResumeUnwind: {
                    //FIXME pop from exception stack
                    builder.CreateResume(builder.CreateLoad(excSlot));
                    break;
                }
                case code::Instruction::Kind::Ret: {
                    const code::Ret &ins = static_cast<const code::Ret &>(ii);
                    builder.CreateRet(temps[ins.getValue().getIndex()]);
                    break;
                }
                case code::Instruction::Kind::RetVoid:
                    builder.CreateRetVoid();
                    break;



                //TODO replace ins.getGlobalVariable().getId() with a map
                //of qore::GlobalVariable * -> llvm::GlobalVariable *
//                case comp::as::Instruction::Kind::ReadLockGlobal: {
//                    comp::as::ReadLockGlobal &ins = static_cast<const comp::as::ReadLockGlobal &>(ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    builder.CreateCall(helper.lf_gv_read_lock, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::ReadUnlockGlobal: {
//                    comp::as::ReadUnlockGlobal &ins = static_cast<const comp::as::ReadUnlockGlobal &>(ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    builder.CreateCall(helper.lf_gv_read_unlock, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::WriteLockGlobal: {
//                    comp::as::WriteLockGlobal &ins = static_cast<const comp::as::WriteLockGlobal &>(ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    builder.CreateCall(helper.lf_gv_write_lock, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::WriteUnlockGlobal: {
//                    comp::as::WriteUnlockGlobal &ins = static_cast<const comp::as::WriteUnlockGlobal &>(ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    builder.CreateCall(helper.lf_gv_write_unlock, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::GetGlobal: {
//                    comp::as::GetGlobal &ins = static_cast<const comp::as::GetGlobal &>(ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    temps[ins.getDest().getIndex()] = builder.CreateCall(helper.lf_gv_get, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::SetGlobal: {
//                    comp::as::SetGlobal &ins = static_cast<const comp::as::SetGlobal &>(ii);
//                    llvm::Value *args[3] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()),
//                            temps[ins.getSrc().getIndex()] };
//                    builder.CreateCall(helper.lf_gv_set, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::MakeGlobal: {
//                    comp::as::MakeGlobal &ins = static_cast<const comp::as::MakeGlobal &>(ii);
//                    llvm::Value *args[4] = {
//                            rtctx,
//                            helper.wrapId(ins.getGlobalVariable().getId()),
//                            helper.wrapBool(ins.getGlobalVariable().getType().isRefCounted()),
//                            temps[ins.getInitValue().getIndex()]
//                    };
//                    builder.CreateCall(helper.lf_createGlobal, args);
//                    break;
//                }
                default:
                    QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ii.getKind()));
            }
        }
    }

    llvm::BasicBlock *mapBlock(const code::Block &b, const char *name) {
        llvm::BasicBlock *&bb = blockMap[&b];
        if (!bb) {
            bb = llvm::BasicBlock::Create(helper.ctx, name, func);
            queue.push_back(&b);
        }
        return bb;
    }

private:
    const Function &f;
    llvm::Function *func;
    Helper &helper;
    llvm::GlobalVariable *rtctx;
    std::vector<llvm::AllocaInst *> locals;
    std::vector<llvm::Value *> temps;
    llvm::Value *excSlot;
    std::map<const code::Block *, llvm::BasicBlock *> blockMap;
    std::vector<const code::Block *> queue;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_FUNCTIONCOMPILER_H_
