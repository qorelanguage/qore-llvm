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
#include "qore/comp/as/is.h"
#include "Helper.h"
#include "qore/core/Function.h"

namespace qore {
namespace cg {

class FunctionCompiler {

public:
    FunctionCompiler(Function &f, llvm::Function *func, Helper &helper, llvm::GlobalVariable *rtctx)
            : f(f), func(func), helper(helper), rtctx(rtctx), locals(f.getLocalCount()), temps(f.getTempCount()),
              excSlot(nullptr) {
        func->setPersonalityFn(llvm::ConstantExpr::getBitCast(helper.lf_personality, helper.lt_char_ptr));
    }

    void x() {
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(helper.ctx, "entry", func);
        llvm::IRBuilder<> builder(entry);
        for (auto &l : locals) {
            l = builder.CreateAlloca(helper.lt_qvalue, nullptr);    //FIXME name
        }
        excSlot = builder.CreateAlloca(helper.lt_exc, nullptr, "exc.slot");

        blockMap[&f.getEntryBlock()] = entry;
        queue.push_back(&f.getEntryBlock());
        while (!queue.empty()) {
            const comp::as::Block *b = queue.back();
            queue.pop_back();
            doBlock(blockMap[b], *b);
        }
        //func->viewCFG();
    }

    llvm::Value *makeCallOrInvoke(llvm::IRBuilder<> &builder, const comp::as::Instruction &ins,
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

    void doBlock(llvm::BasicBlock *bb, const comp::as::Block &block) {
        llvm::IRBuilder<> builder(bb);
        for (auto &ii : block.instructions) {
            switch (ii->getKind()) {
                case comp::as::Instruction::Kind::IntConstant: {
                    comp::as::IntConstant &ins = static_cast<comp::as::IntConstant &>(*ii);
                    temps[ins.getDest().getIndex()] = builder.CreateCall(
                            helper.lf_qint_to_qvalue, llvm::ConstantInt::get(helper.lt_qint, ins.getValue(), true));
                    break;
                }
                case comp::as::Instruction::Kind::GetLocal: {
                    comp::as::GetLocal &ins = static_cast<comp::as::GetLocal &>(*ii);
                    temps[ins.getDest().getIndex()] = builder.CreateLoad(locals[ins.getLocalVariable().getIndex()]);
                    break;
                }
                case comp::as::Instruction::Kind::SetLocal: {
                    comp::as::SetLocal &ins = static_cast<comp::as::SetLocal &>(*ii);
                    builder.CreateStore(temps[ins.getSrc().getIndex()], locals[ins.getLocalVariable().getIndex()]);
                    break;
                }
                case comp::as::Instruction::Kind::LoadString: {
                    comp::as::LoadString &ins = static_cast<comp::as::LoadString &>(*ii);
                    temps[ins.getDest().getIndex()] = helper.loadString(ins.getString());
                    break;
                }
                case comp::as::Instruction::Kind::RefInc: {
                    comp::as::RefInc &ins = static_cast<comp::as::RefInc &>(*ii);
                    builder.CreateCall(helper.lf_incRef, temps[ins.getTemp().getIndex()]);
                    break;
                }
                case comp::as::Instruction::Kind::RefDec: {
                    comp::as::RefDec &ins = static_cast<comp::as::RefDec &>(*ii);
                    makeCallOrInvoke(builder, ins, helper.lf_decRef, temps[ins.getTemp().getIndex()]);
                    break;
                }
                case comp::as::Instruction::Kind::RefDecNoexcept: {
                    comp::as::RefDecNoexcept &ins = static_cast<comp::as::RefDecNoexcept &>(*ii);
                    auto e = llvm::Constant::getNullValue(helper.lt_qvalue); //FIXME top of exception stack
                    llvm::Value *args[2] = { temps[ins.getTemp().getIndex()], e };
                    builder.CreateCall(helper.lf_decRefNoexcept, args);
                    break;
                }
                //TODO replace ins.getGlobalVariable().getId() with a map
                //of qore::GlobalVariable * -> llvm::GlobalVariable *
//                case comp::as::Instruction::Kind::ReadLockGlobal: {
//                    comp::as::ReadLockGlobal &ins = static_cast<comp::as::ReadLockGlobal &>(*ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    builder.CreateCall(helper.lf_gv_read_lock, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::ReadUnlockGlobal: {
//                    comp::as::ReadUnlockGlobal &ins = static_cast<comp::as::ReadUnlockGlobal &>(*ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    builder.CreateCall(helper.lf_gv_read_unlock, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::WriteLockGlobal: {
//                    comp::as::WriteLockGlobal &ins = static_cast<comp::as::WriteLockGlobal &>(*ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    builder.CreateCall(helper.lf_gv_write_lock, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::WriteUnlockGlobal: {
//                    comp::as::WriteUnlockGlobal &ins = static_cast<comp::as::WriteUnlockGlobal &>(*ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    builder.CreateCall(helper.lf_gv_write_unlock, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::GetGlobal: {
//                    comp::as::GetGlobal &ins = static_cast<comp::as::GetGlobal &>(*ii);
//                    llvm::Value *args[2] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()) };
//                    temps[ins.getDest().getIndex()] = builder.CreateCall(helper.lf_gv_get, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::SetGlobal: {
//                    comp::as::SetGlobal &ins = static_cast<comp::as::SetGlobal &>(*ii);
//                    llvm::Value *args[3] = { rtctx, helper.wrapId(ins.getGlobalVariable().getId()),
//                            temps[ins.getSrc().getIndex()] };
//                    builder.CreateCall(helper.lf_gv_set, args);
//                    break;
//                }
//                case comp::as::Instruction::Kind::MakeGlobal: {
//                    comp::as::MakeGlobal &ins = static_cast<comp::as::MakeGlobal &>(*ii);
//                    llvm::Value *args[4] = {
//                            rtctx,
//                            helper.wrapId(ins.getGlobalVariable().getId()),
//                            helper.wrapBool(ins.getGlobalVariable().getType().isRefCounted()),
//                            temps[ins.getInitValue().getIndex()]
//                    };
//                    builder.CreateCall(helper.lf_createGlobal, args);
//                    break;
//                }
                case comp::as::Instruction::Kind::Rethrow: {
                    //FIXME pop from exception stack
                    builder.CreateResume(builder.CreateLoad(excSlot));
                    break;
                }
                case comp::as::Instruction::Kind::BinaryOperator: {
                    comp::as::BinaryOperator &ins = static_cast<comp::as::BinaryOperator &>(*ii);
                    llvm::Value *args[2] = { temps[ins.getLeft().getIndex()], temps[ins.getRight().getIndex()] };
                    temps[ins.getDest().getIndex()] = makeCallOrInvoke(builder, ins,
                            helper.getBinaryOperator(ins.getOperator()), args);
                    break;
                }
                case comp::as::Instruction::Kind::Conversion: {
                    comp::as::Conversion &ins = static_cast<comp::as::Conversion &>(*ii);
                    temps[ins.getDest().getIndex()] = makeCallOrInvoke(builder, ins,
                            helper.getConversion(ins.getConversion()), temps[ins.getArg().getIndex()]);
                    break;
                }
                case comp::as::Instruction::Kind::Ret: {
                    comp::as::Ret &ins = static_cast<comp::as::Ret &>(*ii);
                    builder.CreateRet(temps[ins.getValue().getIndex()]);
                    break;
                }
                case comp::as::Instruction::Kind::RetVoid:
                    builder.CreateRetVoid();
                    break;
                case comp::as::Instruction::Kind::GetArg: {
                    comp::as::GetArg &ins = static_cast<comp::as::GetArg &>(*ii);

                    auto it = func->arg_begin();
                    Index i = ins.getIndex();
                    while (i--) {
                        ++it;
                    }
                    temps[ins.getDest().getIndex()] = it;
                    break;
                }
                case comp::as::Instruction::Kind::Branch: {
                    comp::as::Branch &ins = static_cast<comp::as::Branch &>(*ii);
                    builder.CreateCondBr(
                            builder.CreateCall(helper.lf_qvalue_to_qbool, temps[ins.getCondition().getIndex()]),
                            mapBlock(ins.getTrueDest(), "if.true"),
                            mapBlock(ins.getFalseDest(), "if.false"));
                    break;
                }
                case comp::as::Instruction::Kind::Jump: {
                    comp::as::Jump &ins = static_cast<comp::as::Jump &>(*ii);
                    builder.CreateBr(mapBlock(ins.getDest(), "jump.dest"));
                    break;
                }
                default:
                    QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ii->getKind()));
            }
        }
    }

    llvm::BasicBlock *mapBlock(const comp::as::Block &b, const char *name) {
        llvm::BasicBlock *&bb = blockMap[&b];
        if (!bb) {
            bb = llvm::BasicBlock::Create(helper.ctx, name, func);
            queue.push_back(&b);
        }
        return bb;
    }

private:
    Function &f;
    llvm::Function *func;
    Helper &helper;
    llvm::GlobalVariable *rtctx;
    std::vector<llvm::AllocaInst *> locals;
    std::vector<llvm::Value *> temps;
    llvm::Value *excSlot;
    std::map<const comp::as::Block *, llvm::BasicBlock *> blockMap;
    std::vector<const comp::as::Block *> queue;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_FUNCTIONCOMPILER_H_
