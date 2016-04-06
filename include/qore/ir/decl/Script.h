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
/// \brief Script definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IR_DECL_SCRIPT_H_
#define INCLUDE_QORE_IR_DECL_SCRIPT_H_

#include <string>
#include <vector>
#include "qore/ir/decl/Function.h"
#include "qore/ir/decl/GlobalVariable.h"
#include "qore/ir/decl/StringLiteral.h"
#include "qore/ir/Type.h"
#include "qore/ir/expr/Expression.h"
#include "qore/rt/Types.h"

namespace qore {
namespace ir {

class Script {

public:
    Script() {
    }

    //TODO StrRef, Type, Loc
    const GlobalVariable &createGlobalVariable(std::string name, const Type &type, Expression::Ptr init) {
        GlobalVariable::Ptr ptr = GlobalVariable::create(std::move(name), type, std::move(init));
        GlobalVariable &gv = *ptr;
        globals.push_back(std::move(ptr));
        return gv;
    }

    const StringLiteral &createStringLiteral(std::string value) {
        StringLiteral::Ptr ptr = StringLiteral::create(std::move(value));
        StringLiteral &sl = *ptr;
        strings.push_back(std::move(ptr));
        return sl;
    }

    UserFunction &createUserFunction(std::string name) {
        UserFunction::Ptr ptr = UserFunction::create(std::move(name));
        UserFunction &f = *ptr;
        functions.push_back(std::move(ptr));
        return f;
    }

    const std::vector<StringLiteral::Ptr> &getStringLiterals() const {
        return strings;
    }

    const std::vector<GlobalVariable::Ptr> &getGlobalVariables() const {
        return globals;
    }

    const std::vector<UserFunction::Ptr> &getFunctions() const {
        return functions;
    }

private:
    Script(const Script &) = delete;
    Script(Script &&) = delete;
    Script &operator=(const Script &) = delete;
    Script &operator=(Script &&) = delete;

private:
    std::vector<StringLiteral::Ptr> strings;
    std::vector<GlobalVariable::Ptr> globals;
    std::vector<UserFunction::Ptr> functions;
};

} // namespace ir
} // namespace qore

#endif // INCLUDE_QORE_IR_DECL_SCRIPT_H_
