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
/// \brief LValueEvaluator definition.
///
//------------------------------------------------------------------------------
#ifndef LIB_IN_LVALUEEVALUATOR_H_
#define LIB_IN_LVALUEEVALUATOR_H_

#include "qore/in/Interpreter.h"
#include "qore/ir/expr/Expression.h"
#include "qore/ir/expr/GlobalVariableRefExpression.h"
#include "qore/ir/expr/LocalVariableRefExpression.h"
#include "LValue.h"

namespace qore {
namespace in {

class LValueEvaluator {

public:
    LValueEvaluator(Interpreter &interpreter, Locals &locals);
    LValue evaluate(const ir::Expression &expr);

private:
    LValue evaluate(const ir::GlobalVariableRefExpression &expr);
    LValue evaluate(const ir::LocalVariableRefExpression &expr);

private:
    Interpreter &interpreter;
    Locals &locals;
};

} // namespace in
} // namespace qore

#endif // LIB_IN_LVALUEEVALUATOR_H_
