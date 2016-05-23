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
/// \brief Implements the function compiler.
///
//------------------------------------------------------------------------------
#include "FunctionCompiler.h"
#include <vector>

namespace qore {
namespace cg {

///\cond
class Visitor {

public:
    Visitor(llvm::BasicBlock *b, FunctionContext &ctx) : builder(b), ctx(ctx) {
    }

    void compile(const code::Block &block) {
        for (const code::Instruction &ins : block) {
            ins.accept(*this);
        }
    }

    ///\name Visitor implementation
    ///\{
    using ReturnType = void;

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
        ctx.temps[ins.getDest().getIndex()] = builder.CreateLoad(ctx.strings[&ins.getString()]);
    }

    void visit(const code::GlobalGet &ins) {
        ctx.temps[ins.getDest().getIndex()] = builder.CreateCall(ctx.helper.lf_globalVariable_getValue,
                builder.CreateLoad(ctx.globals[&ins.getGlobalVariable()]));
    }

    void visit(const code::GlobalInit &ins) {
        llvm::Value *args[2] = {
                builder.CreateLoad(ctx.globals[&ins.getGlobalVariable()]),
                ctx.temps[ins.getInitValue().getIndex()]
        };
        builder.CreateCall(ctx.helper.lf_globalVariable_initValue, args);
    }

    void visit(const code::GlobalReadLock &ins) {
        builder.CreateCall(ctx.helper.lf_globalVariable_readLock,
                builder.CreateLoad(ctx.globals[&ins.getGlobalVariable()]));
    }

    void visit(const code::GlobalReadUnlock &ins) {
        builder.CreateCall(ctx.helper.lf_globalVariable_readUnlock,
                builder.CreateLoad(ctx.globals[&ins.getGlobalVariable()]));
    }

    void visit(const code::GlobalSet &ins) {
        llvm::Value *args[2] = {
                builder.CreateLoad(ctx.globals[&ins.getGlobalVariable()]),
                ctx.temps[ins.getSrc().getIndex()]
        };
        builder.CreateCall(ctx.helper.lf_globalVariable_setValue, args);
    }

    void visit(const code::GlobalWriteLock &ins) {
        builder.CreateCall(ctx.helper.lf_globalVariable_writeLock,
                builder.CreateLoad(ctx.globals[&ins.getGlobalVariable()]));
    }

    void visit(const code::GlobalWriteUnlock &ins) {
        builder.CreateCall(ctx.helper.lf_globalVariable_writeUnlock,
                builder.CreateLoad(ctx.globals[&ins.getGlobalVariable()]));
    }

    void visit(const code::InvokeBinaryOperator &ins) {
        llvm::Value *args[2] = { ctx.temps[ins.getLeft().getIndex()], ctx.temps[ins.getRight().getIndex()] };
        ctx.temps[ins.getDest().getIndex()] = makeCallOrInvoke(ins, ctx.helper.getBinaryOperator(ins.getOperator()),
                args);
    }

    void visit(const code::InvokeConversion &ins) {
        ctx.temps[ins.getDest().getIndex()] = makeCallOrInvoke(ins, ctx.helper.getConversion(ins.getConversion()),
                ctx.temps[ins.getArg().getIndex()]);
    }

    void visit(const code::InvokeFunction &ins) {
        std::vector<llvm::Value *> args;
        for (const code::Temp &arg : ins.getArgs()) {
            args.push_back(ctx.temps[arg.getIndex()]);
        }
        ctx.temps[ins.getDest().getIndex()] = makeCallOrInvoke(ins, ctx.functions[&ins.getFunction()], args);
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
        makeCallOrInvoke(ins, ctx.helper.lf_ref_dec, ctx.temps[ins.getTemp().getIndex()]);
    }

    void visit(const code::RefDecNoexcept &ins) {
        builder.CreateCall(ctx.helper.lf_ref_dec_noexcept, ctx.temps[ins.getTemp().getIndex()]);
    }

    void visit(const code::RefInc &ins) {
        builder.CreateCall(ctx.helper.lf_ref_inc, ctx.temps[ins.getTemp().getIndex()]);
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
    ///\}

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
///\endcond

} // namespace cg
} // namespace qore
