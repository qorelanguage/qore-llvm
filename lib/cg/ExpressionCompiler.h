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
#ifndef LIB_CG_EXPRESSIONCOMPILER_H_
#define LIB_CG_EXPRESSIONCOMPILER_H_

#include "qore/ir/expr/AssignmentExpression.h"
#include "qore/ir/expr/CompoundAssignmentExpression.h"
#include "qore/ir/expr/GlobalVariableRefExpression.h"
#include "qore/ir/expr/IntLiteralExpression.h"
#include "qore/ir/expr/InvokeExpression.h"
#include "qore/ir/expr/LifetimeStartExpression.h"
#include "qore/ir/expr/LocalVariableRefExpression.h"
#include "qore/ir/expr/NothingLiteralExpression.h"
#include "qore/ir/expr/StringLiteralRefExpression.h"
#include "Helper.h"
#include "Locals.h"
#include "LValue.h"
#include "Value.h"
#include "ScriptCompiler.h"

namespace qore {
namespace cg {

class ExpressionCompiler {

public:
    ExpressionCompiler(ScriptCompiler &scriptCompiler, Helper &helper, Locals &locals, llvm::IRBuilder<> &builder);

    Value compile(const ir::Expression &expr);
    void compileNoValue(const ir::Expression &expr);

private:
    LValue compileLValue(const ir::Expression &expr);

private:
    Value compile(const ir::GlobalVariableRefExpression &expr);
    Value compile(const ir::IntLiteralExpression &expr);
    Value compile(const ir::InvokeExpression &expr);
    Value compile(const ir::LocalVariableRefExpression &expr);
    Value compile(const ir::NothingLiteralExpression &expr);
    Value compile(const ir::StringLiteralRefExpression &expr);
    void compileNoValue(const ir::AssignmentExpression &expr);
    void compileNoValue(const ir::CompoundAssignmentExpression &expr);
    void compileNoValue(const ir::LifetimeStartExpression &expr);

private:
    ScriptCompiler &scriptCompiler;
    Helper &helper;
    Locals &locals;
    llvm::IRBuilder<> &builder;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_EXPRESSIONCOMPILER_H_
