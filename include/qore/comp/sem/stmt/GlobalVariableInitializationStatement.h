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
/// \brief GlobalVariableInitializationStatement definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_STMT_GLOBALVARIABLEINITIALIZATIONSTATEMENT_H_
#define INCLUDE_QORE_COMP_SEM_STMT_GLOBALVARIABLEINITIALIZATIONSTATEMENT_H_

#include "qore/comp/sem/expr/Expression.h"
#include "qore/comp/sem/stmt/Statement.h"
#include "qore/core/GlobalVariable.h"

namespace qore {
namespace comp {
namespace sem {

class GlobalVariableInitializationStatement : public Statement {

public:
    using Ptr = std::unique_ptr<GlobalVariableInitializationStatement>;

public:
    static Ptr create(GlobalVariable &globalVariable, Expression::Ptr expression) {
        return Ptr(new GlobalVariableInitializationStatement(globalVariable, std::move(expression)));
    }

    Kind getKind() const override {
        return Kind::GlobalVariableInitialization;
    }

    GlobalVariable &getGlobalVariable() const {
        return globalVariable;
    }

    const Expression &getExpression() const {
        return *expression;
    }

private:
    GlobalVariableInitializationStatement(GlobalVariable &globalVariable, Expression::Ptr expression)
            : globalVariable(globalVariable), expression(std::move(expression)) {
    }

private:
    GlobalVariable &globalVariable;
    Expression::Ptr expression;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_STMT_GLOBALVARIABLEINITIALIZATIONSTATEMENT_H_
