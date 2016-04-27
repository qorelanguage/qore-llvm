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
/// \brief StringLiteralRefExpression definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPR_STRINGLITERALREFEXPRESSION_H_
#define INCLUDE_QORE_COMP_SEM_EXPR_STRINGLITERALREFEXPRESSION_H_

#include "qore/comp/sem/expr/Expression.h"
#include "qore/String.h"

namespace qore {
namespace comp {
namespace sem {

class StringLiteralRefExpression : public Expression {

public:
    using Ptr = std::unique_ptr<StringLiteralRefExpression>;

public:
    static Ptr create(qore::String::Ptr string) {
        return Ptr(new StringLiteralRefExpression(std::move(string)));
    }

    Kind getKind() const override {
        return Kind::StringLiteralRef;
    }

    const Type &getType() const override {
        return Type::String;
    }

    qore::String::Ptr getString() const {
        return string.dup();
    }

private:
    explicit StringLiteralRefExpression(qore::String::Ptr string) : string(std::move(string)) {
    }

private:
    qore::String::Ptr string;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPR_STRINGLITERALREFEXPRESSION_H_
