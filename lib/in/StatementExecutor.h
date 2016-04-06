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
/// \brief StatementExecutor definition.
///
//------------------------------------------------------------------------------
#ifndef LIB_IN_STATEMENTEXECUTOR_H_
#define LIB_IN_STATEMENTEXECUTOR_H_

#include "qore/in/Interpreter.h"
#include "qore/ir/stmt/Statement.h"
#include "qore/ir/stmt/ExpressionStatement.h"
#include "Value.h"

namespace qore {
namespace in {

class StatementExecutor {

public:
    enum class Result {
        Normal
    };

public:
    StatementExecutor(Interpreter &interpreter, Locals &locals);

    Result execute(const ir::Statement &stmt);

private:
    Result execute(const ir::ExpressionStatement &stmt);

private:
    Value evaluate(const ir::Expression &expr);

private:
    Interpreter &interpreter;
    Locals &locals;
};

} // namespace in
} // namespace qore

#endif // LIB_IN_STATEMENTEXECUTOR_H_
