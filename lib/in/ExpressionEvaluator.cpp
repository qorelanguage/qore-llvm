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
/// \brief ExpressionEvaluator implementation.
///
//------------------------------------------------------------------------------
#include "ExpressionEvaluator.h"
#include <algorithm>
#include "qore/common/Exceptions.h"
#include "LValueEvaluator.h"

namespace qore {
namespace in {

ExpressionEvaluator::ExpressionEvaluator(Interpreter &interpreter, Locals &locals) : interpreter(interpreter),
        locals(locals) {
}

Value ExpressionEvaluator::evaluate(const ir::Expression &expr) {
    switch (expr.getKind()) {
        case ir::Expression::Kind::GlobalVariableRef:
            return evaluate(static_cast<const ir::GlobalVariableRefExpression &>(expr));
        case ir::Expression::Kind::IntLiteral:
            return evaluate(static_cast<const ir::IntLiteralExpression &>(expr));
        case ir::Expression::Kind::Invoke:
            return evaluate(static_cast<const ir::InvokeExpression &>(expr));
        case ir::Expression::Kind::LocalVariableRef:
            return evaluate(static_cast<const ir::LocalVariableRefExpression &>(expr));
        case ir::Expression::Kind::NothingLiteral:
            return evaluate(static_cast<const ir::NothingLiteralExpression &>(expr));
        case ir::Expression::Kind::StringLiteralRef:
            return evaluate(static_cast<const ir::StringLiteralRefExpression &>(expr));
        default:
            QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(expr.getKind()));
    }
}

void ExpressionEvaluator::evaluateNoValue(const ir::Expression &expr) {
    switch (expr.getKind()) {
        case ir::Expression::Kind::Assignment:
            evaluateNoValue(static_cast<const ir::AssignmentExpression &>(expr));
            return;
        case ir::Expression::Kind::CompoundAssignment:
            evaluateNoValue(static_cast<const ir::CompoundAssignmentExpression &>(expr));
            return;
        case ir::Expression::Kind::LifetimeStart:
            evaluateNoValue(static_cast<const ir::LifetimeStartExpression &>(expr));
            return;
        default:
            QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(expr.getKind()));
    }
}

LValue ExpressionEvaluator::evaluateLValue(const ir::Expression &expr) {
    LValueEvaluator evaluator(interpreter, locals);
    return evaluator.evaluate(expr);
}

Value ExpressionEvaluator::evaluate(const ir::GlobalVariableRefExpression &expr) {
    LOG_FUNCTION();
    rt::GlobalVariable *gv = interpreter.getGlobalVariableValue(expr.getGlobalVariable());
    rt::qvalue p = rt::gv_read_lock(gv);
    if (!expr.getGlobalVariable().getType().isPrimitive()) {
        rt::incRef(p);
    }
    rt::gv_read_unlock(gv);
    return Value(p, !expr.getGlobalVariable().getType().isPrimitive());
}

Value ExpressionEvaluator::evaluate(const ir::IntLiteralExpression &expr) {
    LOG_FUNCTION();
    rt::qvalue v;
    v.i = expr.getValue();
    return Value(v, false);
}

Value ExpressionEvaluator::evaluate(const ir::InvokeExpression &expr) {
    LOG_FUNCTION();

    if (expr.getArgCount() == 1) {
        Value ret;
        try {
            Value arg1 = evaluate(expr.getArg(0));
            try {
                ret = invoke1(expr.getFunction(), arg1.value);
            } catch (rt::Exception &e) {
                arg1.cleanup(e);
                throw;
            }
            arg1.cleanup();
        } catch (rt::Exception &e) {
            ret.cleanup(e);
            throw;
        }
        return ret;
    }
    if (expr.getArgCount() == 2) {
        Value ret;
        try {
            Value arg1 = evaluate(expr.getArg(0));
            try {
                Value arg2 = evaluate(expr.getArg(1));
                try {
                    ret = invoke2(expr.getFunction(), arg1.value, arg2.value);
                } catch (rt::Exception &e) {
                    arg2.cleanup(e);
                    throw;
                }
                arg2.cleanup();
            } catch (rt::Exception &e) {
                arg1.cleanup(e);
                throw;
            }
            arg1.cleanup();
        } catch (rt::Exception &e) {
            ret.cleanup(e);
            throw;
        }
        return ret;
    }
    QORE_NOT_IMPLEMENTED("");
}

Value ExpressionEvaluator::evaluate(const ir::LocalVariableRefExpression &expr) {
    LOG_FUNCTION();
    rt::qvalue p = locals.get(expr.getLocalVariable());
    if (!expr.getLocalVariable().getType().isPrimitive()) {
        rt::incRef(p);
    }
    return Value(p, !expr.getLocalVariable().getType().isPrimitive());
}

Value ExpressionEvaluator::evaluate(const ir::NothingLiteralExpression &expr) {
    LOG_FUNCTION();
    rt::qvalue v;
    v.p = nullptr;
    return Value(v, true);      //or false?
}

Value ExpressionEvaluator::evaluate(const ir::StringLiteralRefExpression &expr) {
    LOG_FUNCTION();
    rt::qvalue s = interpreter.getStringLiteralValue(expr.getStringLiteral());
    rt::incRef(s);
    return Value(s, true);
}

void ExpressionEvaluator::evaluateNoValue(const ir::AssignmentExpression &expr) {
    LOG_FUNCTION();
    Value right = evaluate(expr.getRight());
    try {
        LValue left = evaluateLValue(expr.getLeft());
        std::swap(*left.ptr, right.value);
        left.unlock();
    } catch (rt::Exception &e) {
        right.cleanup(e);
        throw;
    }
    right.cleanup();
}

void ExpressionEvaluator::evaluateNoValue(const ir::CompoundAssignmentExpression &expr) {
    LOG_FUNCTION();
    Value newValue;
    try {
        Value right = evaluate(expr.getRight());
        try {
            LValue left = evaluateLValue(expr.getLeft());
            try {
                newValue = invoke2(expr.getFunction(), *left.ptr, right.value);
                std::swap(*left.ptr, newValue.value);
            } catch (rt::Exception &e) {
                left.unlock();
                throw;
            }
            left.unlock();
        } catch (rt::Exception &e) {
            right.cleanup(e);
            throw;
        }
        right.cleanup();
    } catch (rt::Exception &e) {
        newValue.cleanup(e);
        throw;
    }
    newValue.cleanup();
}

void ExpressionEvaluator::evaluateNoValue(const ir::LifetimeStartExpression &expr) {
    LOG_FUNCTION();
    Value v = evaluate(expr.getInitExpression());
    *locals.getPtr(expr.getLocalVariable()) = v.value;
}

Value ExpressionEvaluator::invoke1(const ir::Function &f, rt::qvalue arg1) {
    if (f.getKind() == ir::Function::Kind::Conversion) {
        auto fff = static_cast<const ir::ConversionFunction &>(f).getPtr();
        return Value(fff(arg1), !f.getRetType().isPrimitive());
    }
    QORE_NOT_IMPLEMENTED("");
}

Value ExpressionEvaluator::invoke2(const ir::Function &f, rt::qvalue arg1, rt::qvalue arg2) {
    if (f.getKind() == ir::Function::Kind::Operator) {
        auto fff = static_cast<const ir::OperatorFunction &>(f).getPtr();
        return Value(fff(arg1, arg2), !f.getRetType().isPrimitive());
    }
    QORE_NOT_IMPLEMENTED("");
}

} // namespace in
} // namespace qore
