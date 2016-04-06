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
/// \brief LValueEvaluator implementation.
///
//------------------------------------------------------------------------------
#include "LValueEvaluator.h"
#include "qore/common/Exceptions.h"
#include "qore/rt/Func.h"

namespace qore {
namespace in {

LValueEvaluator::LValueEvaluator(Interpreter &interpreter, Locals &locals) : interpreter(interpreter),
        locals(locals) {
}

LValue LValueEvaluator::evaluate(const ir::Expression &expr) {
    switch (expr.getKind()) {
        case ir::Expression::Kind::LocalVariableRef:
            return evaluate(static_cast<const ir::LocalVariableRefExpression &>(expr));
        case ir::Expression::Kind::GlobalVariableRef:
            return evaluate(static_cast<const ir::GlobalVariableRefExpression &>(expr));
        default:
            QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(expr.getKind()));
    }
}

LValue LValueEvaluator::evaluate(const ir::GlobalVariableRefExpression &expr) {
    LOG_FUNCTION();
    rt::GlobalVariable *gv = interpreter.getGlobalVariableValue(expr.getGlobalVariable());
    rt::qvalue *ptr = qore::rt::gv_write_lock(gv);
    return LValue(ptr, expr.getGlobalVariable().getType(), [&gv](){qore::rt::gv_write_unlock(gv);});
}

LValue LValueEvaluator::evaluate(const ir::LocalVariableRefExpression &expr) {
    LOG_FUNCTION();
    return LValue(locals.getPtr(expr.getLocalVariable()), expr.getLocalVariable().getType());
}

} // namespace in
} // namespace qore
