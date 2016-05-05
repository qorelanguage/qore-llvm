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
#include "FunctionCompiler.h"
#include <vector>

namespace qore {
namespace cg {

class Visitor {
public:
    using ReturnType = void;

public:
    Visitor(llvm::BasicBlock *b, FunctionContext &ctx) : builder(b), ctx(ctx) {
    }

    void compile(const code::Block &block) {
        for (const code::Instruction &ins : block) {
            ins.accept(*this);
        }
    }

    void visit(const code::Branch &ins) {
        builder.CreateCondBr(
                builder.CreateCall(ctx.helper.lf_qvalue_to_qbool, ctx.temps[ins.getCondition().getIndex()]),
                mapBlock(ins.getTrueDest(), "if.true"),
                mapBlock(ins.getFalseDest(), "if.false"));
    }

    void visit(const code::ConstInt &ins) {
        ctx.temps[ins.getDest().getIndex()] = builder.CreateCall(ctx.helper.lf_qint_to_qvalue,
                llvm::ConstantInt::get(ctx.helper.lt_qint, ins.getValue(), true));
    }

    void visit(const code::ConstString &ins) {
        ctx.temps[ins.getDest().getIndex()] = builder.CreateLoad(ctx.helper.getString(ins.getString()));
    }

    //TODO replace ins.getGlobalVariable().getId() with a map
    //of qore::GlobalVariable * -> llvm::GlobalVariable *
//    void visit(const code::GlobalGet &ins) {
//        llvm::Value *args[2] = { rtctx, ctx.helper.wrapId(ins.getGlobalVariable().getId()) };
//        ctx.temps[ins.getDest().getIndex()] = builder.CreateCall(ctx.helper.lf_gv_get, args);
//    }
//    void visit(const code::GlobalInit &ins) {
//        llvm::Value *args[4] = {
//                rtctx,
//                ctx.helper.wrapId(ins.getGlobalVariable().getId()),
//                ctx.helper.wrapBool(ins.getGlobalVariable().getType().isRefCounted()),
//                ctx.temps[ins.getInitValue().getIndex()]
//        };
//        builder.CreateCall(ctx.helper.lf_createGlobal, args);
//    }
//    void visit(const code::GlobalReadLock &ins) {
//        llvm::Value *args[2] = { rtctx, ctx.helper.wrapId(ins.getGlobalVariable().getId()) };
//        builder.CreateCall(ctx.helper.lf_gv_read_lock, args);
//    }
//    void visit(const code::GlobalReadUnlock &ins) {
//        llvm::Value *args[2] = { rtctx, ctx.helper.wrapId(ins.getGlobalVariable().getId()) };
//        builder.CreateCall(ctx.helper.lf_gv_read_unlock, args);
//    }
//    void visit(const code::GlobalSet &ins) {
//        llvm::Value *args[3] = { rtctx, ctx.helper.wrapId(ins.getGlobalVariable().getId()),
//                ctx.temps[ins.getSrc().getIndex()] };
//        builder.CreateCall(ctx.helper.lf_gv_set, args);
//    }
//    void visit(const code::GlobalWriteLock &ins) {
//        llvm::Value *args[2] = { rtctx, ctx.helper.wrapId(ins.getGlobalVariable().getId()) };
//        builder.CreateCall(ctx.helper.lf_gv_write_lock, args);
//    }
//    void visit(const code::GlobalWriteUnlock &ins) {
//        llvm::Value *args[2] = { rtctx, ctx.helper.wrapId(ins.getGlobalVariable().getId()) };
//        builder.CreateCall(ctx.helper.lf_gv_write_unlock, args);
//    }

    void visit(const code::InvokeBinaryOperator &ins) {
        llvm::Value *args[2] = { ctx.temps[ins.getLeft().getIndex()], ctx.temps[ins.getRight().getIndex()] };
        ctx.temps[ins.getDest().getIndex()] = makeCallOrInvoke(ins, ctx.helper.getBinaryOperator(ins.getOperator()),
                args);
    }

    void visit(const code::InvokeConversion &ins) {
        ctx.temps[ins.getDest().getIndex()] = makeCallOrInvoke(ins, ctx.helper.getConversion(ins.getConversion()),
                ctx.temps[ins.getArg().getIndex()]);
    }

    void visit(const code::Jump &ins) {
        builder.CreateBr(mapBlock(ins.getDest(), "jump.dest"));
    }

    void visit(const code::LocalGet &ins) {
        ctx.temps[ins.getDest().getIndex()] = builder.CreateLoad(ctx.locals[ins.getLocalVariable().getIndex()]);
    }

    void visit(const code::LocalSet &ins) {
        builder.CreateStore(ctx.temps[ins.getSrc().getIndex()], ctx.locals[ins.getLocalVariable().getIndex()]);
    }

    void visit(const code::RefDec &ins) {
        makeCallOrInvoke(ins, ctx.helper.lf_decRef, ctx.temps[ins.getTemp().getIndex()]);
    }

    void visit(const code::RefDecNoexcept &ins) {
        auto e = llvm::Constant::getNullValue(ctx.helper.lt_qvalue); //FIXME top of exception stack
        llvm::Value *args[2] = { ctx.temps[ins.getTemp().getIndex()], e };
        builder.CreateCall(ctx.helper.lf_decRefNoexcept, args);
    }

    void visit(const code::RefInc &ins) {
        builder.CreateCall(ctx.helper.lf_incRef, ctx.temps[ins.getTemp().getIndex()]);
    }

    void visit(const code::ResumeUnwind &ins) {
        //FIXME pop from exception stack
        builder.CreateResume(builder.CreateLoad(ctx.excSlot));
    }

    void visit(const code::Ret &ins) {
        builder.CreateRet(ctx.temps[ins.getValue().getIndex()]);
    }

    void visit(const code::RetVoid &ins) {
        builder.CreateRetVoid();
    }

    void visit(const code::Instruction &ins) {
        QORE_NOT_IMPLEMENTED("Instruction " << static_cast<int>(ins.getKind()));
    }

private:
    llvm::Value *makeCallOrInvoke(const code::Instruction &ins, llvm::Function *f, llvm::ArrayRef<llvm::Value *> args) {
        if (!ins.getLpad()) {
            return builder.CreateCall(f, args);
        }

        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx.helper.ctx, "bb", builder.GetInsertBlock()->getParent());
        llvm::BasicBlock *lpad = llvm::BasicBlock::Create(ctx.helper.ctx, "lpad",
                builder.GetInsertBlock()->getParent());
        llvm::Value *ret = builder.CreateInvoke(f, bb, lpad, args);
        builder.SetInsertPoint(bb);

        Visitor v(lpad, ctx);
        v.landingPad();
        v.compile(*ins.getLpad());

        return ret;
    }

    void landingPad() {
        llvm::LandingPadInst *l = builder.CreateLandingPad(ctx.helper.lt_exc, 1);
        l->setCleanup(true);
        builder.CreateStore(l, ctx.excSlot);
        //FIXME extract exception and use it in decRefNoExcept
    }

    llvm::BasicBlock *mapBlock(const code::Block &b, const char *name) {
        llvm::BasicBlock *&bb = ctx.blockMap[&b];
        if (!bb) {
            bb = llvm::BasicBlock::Create(ctx.helper.ctx, name, builder.GetInsertBlock()->getParent());
            ctx.queue.push_back(&b);
        }
        return bb;
    }

private:
    llvm::IRBuilder<> builder;
    FunctionContext &ctx;
};


void FunctionCompiler::doBlock(const code::Block &block) {
    Visitor v(ctx.blockMap[&block], ctx);
    v.compile(block);
}

} // namespace cg
} // namespace qore
