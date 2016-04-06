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
/// \brief GlobalVariable definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IR_DECL_GLOBALVARIABLE_H_
#define INCLUDE_QORE_IR_DECL_GLOBALVARIABLE_H_

#include <memory>
#include <string>
#include "qore/ir/expr/Expression.h"
#include "qore/ir/Type.h"

namespace qore {
namespace ir {

class GlobalVariable {

public:
    using Ptr = std::unique_ptr<GlobalVariable>;

public:
    static Ptr create(std::string name, const Type &type, Expression::Ptr initExpression) {
        return Ptr(new GlobalVariable(std::move(name), type, std::move(initExpression)));
    }

public:
    const std::string &getName() const {
        return name;
    }

    const Type &getType() const {
        return type;
    }

    const Expression &getInitExpression() const {
        return *initExpression;
    }

private:
    GlobalVariable(std::string name, const Type &type, Expression::Ptr initExpression) : name(std::move(name)),
            type(type), initExpression(std::move(initExpression)) {
    }

private:
    GlobalVariable(const GlobalVariable &) = delete;
    GlobalVariable(GlobalVariable &&) = delete;
    GlobalVariable &operator=(const GlobalVariable &) = delete;
    GlobalVariable &operator=(GlobalVariable &&) = delete;

private:
    std::string name;
    const Type &type;
    Expression::Ptr initExpression;
};

} // namespace ir
} // namespace qore

#endif // INCLUDE_QORE_IR_DECL_GLOBALVARIABLE_H_
