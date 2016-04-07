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
#include "ExpressionCompiler.h"
#include <vector>
#include "LValueCompiler.h"

namespace qore {
namespace cg {

ExpressionCompiler::ExpressionCompiler(ScriptCompiler &scriptCompiler, Helper &helper, Locals &locals,
        llvm::IRBuilder<> &builder) : scriptCompiler(scriptCompiler), helper(helper), locals(locals), builder(builder) {
}

Value ExpressionCompiler::compile(const ir::Expression &expr) {
    switch (expr.getKind()) {
        case ir::Expression::Kind::GlobalVariableRef:
            return compile(static_cast<const ir::GlobalVariableRefExpression &>(expr));
        case ir::Expression::Kind::Invoke:
            return compile(static_cast<const ir::InvokeExpression &>(expr));
        case ir::Expression::Kind::IntLiteral:
            return compile(static_cast<const ir::IntLiteralExpression &>(expr));
        case ir::Expression::Kind::LocalVariableRef:
            return compile(static_cast<const ir::LocalVariableRefExpression &>(expr));
        case ir::Expression::Kind::NothingLiteral:
            return compile(static_cast<const ir::NothingLiteralExpression &>(expr));
        case ir::Expression::Kind::StringLiteralRef:
            return compile(static_cast<const ir::StringLiteralRefExpression &>(expr));
        default:
            QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(expr.getKind()));
    }
}

void ExpressionCompiler::compileNoValue(const ir::Expression &expr) {
    switch (expr.getKind()) {
        case ir::Expression::Kind::Assignment:
            compileNoValue(static_cast<const ir::AssignmentExpression &>(expr));
            return;
        case ir::Expression::Kind::CompoundAssignment:
            compileNoValue(static_cast<const ir::CompoundAssignmentExpression &>(expr));
            return;
        case ir::Expression::Kind::LifetimeStart:
            compileNoValue(static_cast<const ir::LifetimeStartExpression &>(expr));
            return;
        default:
            QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(expr.getKind()));
    }
}

LValue ExpressionCompiler::compileLValue(const ir::Expression &expr) {
    LValueCompiler lvalueCompiler(scriptCompiler, helper, locals, builder);
    return lvalueCompiler.compile(expr);
}

Value ExpressionCompiler::compile(const ir::GlobalVariableRefExpression &expr) {
    LOG_FUNCTION();
    llvm::Value *g = builder.CreateLoad(scriptCompiler.getGlobal(expr.getGlobalVariable()));
    llvm::Value *qv = builder.CreateCall(helper.lf_gv_read_lock, g);
    if (expr.getGlobalVariable().getType().rtType == qore::rt::qvalue_type::Ptr) {
        builder.CreateCall(helper.lf_incRef, qv);
    }
    builder.CreateCall(helper.lf_gv_read_unlock, g);
    return Value(expr.getGlobalVariable().getType().rtType, qv);
}

Value ExpressionCompiler::compile(const ir::IntLiteralExpression &expr) {
    LOG_FUNCTION();

    return Value(rt::qvalue_type::Int, builder.CreateCall(helper.lf_qint_to_qvalue,
            llvm::ConstantInt::get(helper.lt_qint, expr.getValue(), true)));
}

Value ExpressionCompiler::compile(const ir::InvokeExpression &expr) {
    LOG_FUNCTION();
    const ir::Function &f = expr.getFunction();
    assert(f.getArgCount() == expr.getArgCount());

    std::vector<llvm::Value *> args;
    for (ir::Function::Index i = 0; i < f.getArgCount(); ++i) {
        Value v = compile(expr.getArg(i));
        //assert that v.type == f.getArgType(i)
        args.push_back(v.value);
    }
    llvm::Value *r = scriptCompiler.call(builder, f, args);
    for (ir::Function::Index i = f.getArgCount(); i > 0; --i) {
        if (f.getArgType(i - 1).rtType == rt::qvalue_type::Ptr) {
            builder.CreateCall(helper.lf_decRef, args[i - 1]);
        }
    }
    return Value(f.getRetType().rtType, r);
}

Value ExpressionCompiler::compile(const ir::LocalVariableRefExpression &expr) {
    LOG_FUNCTION();
    llvm::Value *v = builder.CreateLoad(locals.get(expr.getLocalVariable()));
    if (expr.getLocalVariable().getType().rtType == qore::rt::qvalue_type::Ptr) {
        builder.CreateCall(helper.lf_incRef, v);
    }
    return Value(expr.getLocalVariable().getType().rtType, v);
}

Value ExpressionCompiler::compile(const ir::NothingLiteralExpression &expr) {
    LOG_FUNCTION();
    return Value(rt::qvalue_type::Ptr, llvm::Constant::getNullValue(helper.lt_qptr));
}

Value ExpressionCompiler::compile(const ir::StringLiteralRefExpression &expr) {
    LOG_FUNCTION();
    llvm::LoadInst *load = builder.CreateLoad(scriptCompiler.getString(expr.getStringLiteral()));
    builder.CreateCall(helper.lf_incRef, load);
    return Value(rt::qvalue_type::Ptr, load);
}

void ExpressionCompiler::compileNoValue(const ir::AssignmentExpression &expr) {
    LOG_FUNCTION();
    Value newValue = compile(expr.getRight());
    //push cleanup of newValue
    LValue left = compileLValue(expr.getLeft());
    assert(left.type == newValue.type);
    llvm::Value *ptr = left.lock();
    //push unlock of var
    llvm::Value *oldValue = builder.CreateLoad(ptr);
    builder.CreateStore(newValue.value, ptr);
    //pop cleanup of newValue
    //push cleanup of oldValue
    left.unlock();
    //pop unlock of var

    if (left.type == qore::rt::qvalue_type::Ptr) {
        builder.CreateCall(helper.lf_decRef, oldValue);
    }
    //pop cleanup of oldValue
}

void ExpressionCompiler::compileNoValue(const ir::CompoundAssignmentExpression &expr) {
    LOG_FUNCTION();
    Value right = compile(expr.getRight());
    //push cleanup of right
    LValue lll = compileLValue(expr.getLeft());
    llvm::Value *ptr = lll.lock();
    //push unlock of var

    llvm::Value *old = builder.CreateLoad(ptr);
    llvm::Value *args[2] = {old, right.value};

    assert(expr.getFunction().getArgCount() == 2);

    llvm::Value *sum = scriptCompiler.call(builder, expr.getFunction(), args);
    //push cleanup of sum

    builder.CreateStore(sum, ptr);
    //pop cleanup of sum
    //push cleanup of old

    lll.unlock();
    //pop unlock of var

    if (lll.type == rt::qvalue_type::Ptr) {
        builder.CreateCall(helper.lf_decRef, old);
    }
    //pop cleanup of old

    if (right.type == rt::qvalue_type::Ptr) {
        builder.CreateCall(helper.lf_decRef, right.value);
    }
    //pop cleanup of right
}

void ExpressionCompiler::compileNoValue(const ir::LifetimeStartExpression &expr) {
    LOG_FUNCTION();
    Value init = compile(expr.getInitExpression());
    assert(init.type == expr.getLocalVariable().getType().rtType);
    builder.CreateStore(init.value, locals.get(expr.getLocalVariable()));
    //push cleanup of locals[stmt->v]
}

} // namespace cg
} // namespace qore
