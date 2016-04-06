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
#include "StatementCompiler.h"
#include "ExpressionCompiler.h"

namespace qore {
namespace cg {

StatementCompiler::StatementCompiler(ScriptCompiler &scriptCompiler, Helper &helper, Locals &locals,
        llvm::IRBuilder<> builder) : scriptCompiler(scriptCompiler), helper(helper), locals(locals), builder(builder) {
}

void StatementCompiler::compile(const ir::Statement &stmt) {
    switch (stmt.getKind()) {
        case ir::Statement::Kind::Expression:
            return compile(static_cast<const ir::ExpressionStatement &>(stmt));
        default:
            QORE_UNREACHABLE("Invalid Statement::Kind: " << static_cast<int>(stmt.getKind()));
    }
}

void StatementCompiler::compile(const ir::ExpressionStatement &stmt) {
    LOG_FUNCTION();
    ExpressionCompiler expressionCompiler(scriptCompiler, helper, locals, builder);
    expressionCompiler.compileNoValue(stmt.getExpression());
}

Value StatementCompiler::evaluate(const ir::Expression &expr) {
    ExpressionCompiler expressionCompiler(scriptCompiler, helper, locals, builder);
    return expressionCompiler.compile(expr);
}

} // namespace cg
} // namespace qore
