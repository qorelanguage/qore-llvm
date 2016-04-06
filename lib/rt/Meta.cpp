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
/// \brief TODO description
///
//------------------------------------------------------------------------------
#include "qore/rt/Meta.h"
#include "qore/common/Exceptions.h"

namespace qore {
namespace rt {
namespace meta {

Operator findOperatorAdd(Type l, Type r) {
    if (l == Type::Int) {
        if (r == Type::Int) {
            return Operator::IntPlusInt;
        } else if (r == Type::String) {
            return Operator::StringPlusString;
        }
    } else if (l == Type::String) {
        if (r == Type::Int || r == Type::String) {
            return Operator::StringPlusString;
        }
    }
    QORE_NOT_IMPLEMENTED("Operator " << static_cast<int>(l) << " + " << static_cast<int>(r));
}

Conversion findConversion(Type src, Type dest) {
    if (src == dest) {
        return Conversion::Identity;
    }
    if (src == Type::Int && dest == Type::String) {
        return Conversion::IntToString;
    }
    QORE_NOT_IMPLEMENTED("Conversion " << static_cast<int>(src) << " to " << static_cast<int>(dest));
}

} // namespace meta
} // namespace rt
} // namespace qore
