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
/// \brief ReturnStatement definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_STMT_RETURNSTATEMENT_H_
#define INCLUDE_QORE_COMP_SEM_STMT_RETURNSTATEMENT_H_

#include "qore/comp/sem/expr/Expression.h"
#include "qore/comp/sem/stmt/Statement.h"

namespace qore {
namespace comp {
namespace sem {

class ReturnStatement : public Statement {

public:
    using Ptr = std::unique_ptr<ReturnStatement>;

public:
    static Ptr create(Expression::Ptr expression = nullptr) {
        return Ptr(new ReturnStatement(std::move(expression)));
    }

    Kind getKind() const override {
        return Kind::Return;
    }

    const Expression *getExpression() const {
        return expression.get();
    }

private:
    explicit ReturnStatement(Expression::Ptr expression) : expression(std::move(expression)) {
    }

private:
    Expression::Ptr expression;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_STMT_RETURNSTATEMENT_H_
