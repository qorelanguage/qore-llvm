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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#include "qore/comp/sem/Core.h"
#include <string>
#include "qore/comp/sem/FunctionGroupInfo.h"
#include "qore/comp/sem/expr/IntLiteralExpression.h"
#include "qore/comp/sem/expr/NothingLiteralExpression.h"
#include "qore/comp/sem/expr/StringLiteralRefExpression.h"

namespace qore {
namespace comp {
namespace sem {

Core::Core(Context &ctx) : ctx(ctx) {
    builtinTypes[ctx.getStringTable().put("int")] = std::make_pair(&Type::Int, &Type::IntOpt);
    builtinTypes[ctx.getStringTable().put("string")] = std::make_pair(&Type::String, &Type::StringOpt);
    builtinTypes[ctx.getStringTable().put("any")] = std::make_pair(&Type::Any, nullptr);
}

Expression::Ptr Core::defaultFor(const Type &type) {
    if (type == Type::String) {
        return StringLiteralRefExpression::create(createStringLiteral(""));
    }
    if (type == Type::Int) {
        return IntLiteralExpression::create(0);
    }
    if (type.acceptsNothing()) {
        return NothingLiteralExpression::create();
    }
    QORE_NOT_IMPLEMENTED("Default value");
}

qore::String &Core::createStringLiteral(const std::string &value) {
    qore::String *&ref = strings[value];
    if (!ref) {
        ref = &ctx.getEnv().addString(std::move(value));
    }
    return *ref;
}

const Type *Core::getBuiltinType(const ast::Name::Id &name, bool asterisk) const {
    auto it = builtinTypes.find(name.str);
    if (it == builtinTypes.end()) {
        return nullptr;
    }

    if (asterisk) {
        if (!it->second.second) {
            //FIXME report error (any with asterisk)
            return &Type::Error;
        }
        return it->second.second;
    }
    return it->second.first;
}

} // namespace sem
} // namespace comp
} // namespace qore
