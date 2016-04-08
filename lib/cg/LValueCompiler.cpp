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
#include "LValueCompiler.h"

namespace qore {
namespace cg {

LValueCompiler::LValueCompiler(ScriptCompiler &scriptCompiler, Helper &helper, Locals &locals,
        llvm::IRBuilder<> &builder) : scriptCompiler(scriptCompiler), helper(helper), locals(locals), builder(builder) {
}

LValue LValueCompiler::compile(const ir::Expression &expr) {
    switch (expr.getKind()) {
        case ir::Expression::Kind::GlobalVariableRef:
            return compile(static_cast<const ir::GlobalVariableRefExpression &>(expr));
        case ir::Expression::Kind::LocalVariableRef:
            return compile(static_cast<const ir::LocalVariableRefExpression &>(expr));
        default:
            QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(expr.getKind()));
    }
}

LValue LValueCompiler::compile(const ir::LocalVariableRefExpression &expr) {
    LOG_FUNCTION();
    llvm::Value *ptr = locals.get(expr.getLocalVariable());
    return LValue([ptr](){return ptr;});
}

LValue LValueCompiler::compile(const ir::GlobalVariableRefExpression &expr) {
    LOG_FUNCTION();
    llvm::Value *g = builder.CreateLoad(scriptCompiler.getGlobal(expr.getGlobalVariable()));

    llvm::IRBuilder<> &b = builder;
    Helper &h = helper;
    return LValue(
            [&b, &h, g](){return b.CreateCall(h.lf_gv_write_lock, g); },
            [&b, &h, g](){b.CreateCall(h.lf_gv_write_unlock, g);});
}

} // namespace cg
} // namespace qore



