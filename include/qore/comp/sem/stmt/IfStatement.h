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
/// \brief IfStatement definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_STMT_IFSTATEMENT_H_
#define INCLUDE_QORE_COMP_SEM_STMT_IFSTATEMENT_H_

#include <cassert>
#include "qore/comp/sem/expr/Expression.h"
#include "qore/comp/sem/stmt/Statement.h"

namespace qore {
namespace comp {
namespace sem {

class IfStatement : public Statement {

public:
    using Ptr = std::unique_ptr<IfStatement>;

public:
    static Ptr create(Expression::Ptr condition, Statement::Ptr trueBranch, Statement::Ptr falseBranch) {
        assert(condition);
        assert(trueBranch);
        return Ptr(new IfStatement(std::move(condition), std::move(trueBranch), std::move(falseBranch)));
    }

    Kind getKind() const override {
        return Kind::If;
    }

    const Expression &getCondition() const {
        return *condition;
    }

    const Statement &getTrueBranch() const {
        return *trueBranch;
    }

    const Statement *getFalseBranch() const {
        return falseBranch.get();
    }

private:
    IfStatement(Expression::Ptr condition, Statement::Ptr trueBranch, Statement::Ptr falseBranch)
            : condition(std::move(condition)), trueBranch(std::move(trueBranch)), falseBranch(std::move(falseBranch)) {
    }

private:
    Expression::Ptr condition;
    Statement::Ptr trueBranch;
    Statement::Ptr falseBranch;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_STMT_IFSTATEMENT_H_
